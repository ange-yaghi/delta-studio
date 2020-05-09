#include "../include/yds_audio_streaming.h"

#include "../include/yds_audio_source.h"
#include "../include/yds_audio_file.h"

ysStreamingAudio::ysStreamingAudio() : ysObject("ysStreamingAudio") {
	m_file = nullptr;
	m_source = nullptr;
	m_fileOffset = 0;

	m_subdivisions = 0;
	m_currentWriteSubdivision = 0;
	m_currentReadSubdivision = 0;
}

ysStreamingAudio::~ysStreamingAudio() {
	/* void */
}

ysError ysStreamingAudio::AttachAudioFile(ysAudioFile *file) {
	YDS_ERROR_DECLARE("AttachAudioFile");

	ysError error;
	m_file = file;

	error = UpdateMetrics();
	if (error != ysError::YDS_NO_ERROR) m_file = nullptr;

	return YDS_ERROR_RETURN(error);
}

ysError ysStreamingAudio::AttachAudioSource(ysAudioSource *source) {
	YDS_ERROR_DECLARE("AttachAudioBuffer");

	ysError error;
	m_source = source;

	error = UpdateMetrics();
	if (error != ysError::YDS_NO_ERROR) m_source = nullptr;

	return error;
}

ysError ysStreamingAudio::UpdateMetrics() {
	YDS_ERROR_DECLARE("UpdateMetrics");

	if (m_file == nullptr) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
	if (m_source == nullptr) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
	if (m_file->GetBufferSize() > m_source->GetBufferSize()) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_BUFFER_AND_FILE);
	if (m_buffer->GetBufferSize() % m_file->GetBufferSize() > 0) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_BUFFER_AND_FILE);

	m_subdivisions = m_buffer->GetBufferSize() / m_file->GetBufferSize();

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

ysError ysStreamingAudio::Seek(SampleOffset offset) {
	YDS_ERROR_DECLARE("Seek");

	if (m_file == nullptr) return YDS_ERROR_RETURN(ysError::YDS_NO_FILE);
	if (m_buffer == nullptr) return YDS_ERROR_RETURN(ysError::YDS_NO_AUDIO_BUFFER);
	if (m_file->GetBufferSize() == 0) return YDS_ERROR_RETURN(ysError::YDS_NO_FILE_BUFFER);

	ysAudioBuffer::Mode mode = m_buffer->GetBufferMode();
	m_buffer->SetMode(ysAudioBuffer::Mode::Stop);
	m_buffer->Seek(0);

	m_fileOffset = offset;
	ysError error = InitializeBuffer();

	m_buffer->SetMode(mode);

	return YDS_ERROR_RETURN(error);
}

ysError ysStreamingAudio::InitializeBuffer() {
	YDS_ERROR_DECLARE("InitializeBuffer");

	if (m_file == nullptr) return YDS_ERROR_RETURN(ysError::YDS_NO_FILE);
	if (m_buffer == nullptr) return YDS_ERROR_RETURN(ysError::YDS_NO_AUDIO_BUFFER);
	if (m_file->GetBufferSize() == 0) return YDS_ERROR_RETURN(ysError::YDS_NO_FILE_BUFFER);

	void *audioBuffer;
	SampleOffset audioBufferSize;
	m_buffer->LockEntireBuffer(&audioBuffer, &audioBufferSize);

	unsigned int bufferOffset;

	for(int i = 0; i < m_subdivisions; i++) {
		bufferOffset = m_buffer->GetAudioParameters()->GetSizeFromSamples(i * m_file->GetBufferSize());

		m_file->FillBuffer(m_fileOffset);
		const void *fileBuffer = m_file->GetBuffer();

		memcpy(((char *)audioBuffer) + bufferOffset, fileBuffer, m_file->GetAudioParameters()->GetSizeFromSamples(m_file->GetBufferSize()));
		
		m_fileOffset += m_file->GetBufferSize();
	}

	m_buffer->UnlockBufferSegments(audioBuffer, audioBufferSize, NULL, NULL);

	m_currentWriteSubdivision = 0;
	m_currentReadSubdivision = 0;

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysStreamingAudio::Transfer() {
	m_file->FillBuffer(m_fileOffset);

	SampleOffset writeOffset = m_currentWriteSubdivision * m_file->GetBufferSize();

	void *segment1;
	SampleOffset size1;

	void *segment2;
	SampleOffset size2;

	const void *fileBuffer = m_file->GetBuffer();

	ysError error = m_buffer->LockBufferSegment(writeOffset, m_file->GetBufferSize(), &segment1, &size1, &segment2, &size2);

	SampleOffset writeCursor = m_buffer->GetCurrentWritePosition();

	if (segment1 != nullptr) {
		unsigned int size = m_buffer->GetAudioParameters()->GetSizeFromSamples(size1);
		memcpy(segment1, fileBuffer, size);
	}

	if (segment2 != nullptr) {
		memcpy(segment2, ((char *)fileBuffer) + m_buffer->GetAudioParameters()->GetSizeFromSamples(size1), m_buffer->GetAudioParameters()->GetSizeFromSamples(size2));
	}

	m_buffer->UnlockBufferSegments(segment1, size1, segment2, size2);
}

ysError ysStreamingAudio::Update() {
	YDS_ERROR_DECLARE("Update");

	int readSubdivision;
	SampleOffset readPosition;

	readPosition = m_buffer->GetCurrentPosition();
	readSubdivision = readPosition / m_file->GetBufferSize();

	if (readSubdivision != m_currentReadSubdivision) {
		if (readSubdivision != ((m_currentReadSubdivision + 1) % m_subdivisions)) {
			// TEMP
			// Weird jump, don't know how to handle this yet
		}

		m_currentReadSubdivision = readSubdivision;

		Transfer();
		m_currentWriteSubdivision += 1;
		m_currentWriteSubdivision %= m_subdivisions;

		m_fileOffset += m_file->GetBufferSize();
	}

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}
