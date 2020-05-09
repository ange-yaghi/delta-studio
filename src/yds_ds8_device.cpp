#include "../include/yds_ds8_device.h"

#include "../include/yds_ds8_audio_buffer.h"

ysDS8Device::ysDS8Device() : ysAudioDevice(API::DirectSound8) {
	memset(&m_guid, 0, sizeof(GUID));
	m_device = nullptr;
}

ysDS8Device::~ysDS8Device() {
    /* void */
}

ysAudioBuffer *ysDS8Device::CreateBuffer(const ysAudioParameters *parameters, SampleOffset size) {
	ysDS8AudioBuffer *newBuffer = m_audioBuffers.NewGeneric<ysDS8AudioBuffer>();

	// --------------------------------------------------------
	// TEMP
	// --------------------------------------------------------
	WAVEFORMATEX wfx; 
	DSBUFFERDESC dsbdesc; 
	//LPDIRECTSOUNDBUFFER pDsb = NULL;
	HRESULT hr; 
 
	// Set up WAV format structure. 

	memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
	wfx.wFormatTag = WAVE_FORMAT_PCM; 
	wfx.nChannels = parameters->m_channelCount; 
	wfx.nSamplesPerSec = parameters->m_sampleRate;
	wfx.nBlockAlign = parameters->m_channelCount * (parameters->m_bitsPerSample / 8); 
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 
	wfx.wBitsPerSample = parameters->m_bitsPerSample; 

	newBuffer->m_bufferSize = size;
	newBuffer->m_audioParameters = *parameters;
 
	// Set up DSBUFFERDESC structure. 
 
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
	dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
	dsbdesc.dwFlags = 
	DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY
	| DSBCAPS_GLOBALFOCUS; 
	dsbdesc.dwBufferBytes = parameters->GetSizeFromSamples(size); 
	dsbdesc.lpwfxFormat = &wfx; 
 
	// Create buffer. 
 
	hr = m_device->CreateSoundBuffer(&dsbdesc, &newBuffer->m_buffer, NULL); 
	if (SUCCEEDED(hr)) { 
		hr = newBuffer->m_buffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&newBuffer->m_buffer);
		newBuffer->m_buffer->Release();
	} 
	else {
        /* void */
	}

	// --------------------------------------------------------

	return newBuffer;
}

ysAudioBuffer *ysDS8Device::CreateDuplicate(ysAudioBuffer *buffer) {
	ysDS8AudioBuffer *ds8Buffer = static_cast<ysDS8AudioBuffer *>(buffer);
	ysDS8AudioBuffer *newBuffer = m_audioBuffers.NewGeneric<ysDS8AudioBuffer>();

	m_device->DuplicateSoundBuffer(ds8Buffer->m_buffer, &newBuffer->m_buffer);

	return newBuffer;
}

ysError ysDS8Device::DestroyAudioBuffer(ysAudioBuffer *&buffer) {
	YDS_ERROR_DECLARE("DestroyAudioBuffer");

	if (buffer == NULL) return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
	if (!CheckCompatibility(buffer)) return YDS_ERROR_RETURN(ysError::YDS_INCOMPATIBLE_PLATFORMS);

	ysDS8AudioBuffer *ds8Buffer = static_cast<ysDS8AudioBuffer *>(buffer);

	ds8Buffer->m_buffer->Release();

	YDS_NESTED_ERROR_CALL( ysAudioDevice::DestroyAudioBuffer(buffer) );

	return YDS_ERROR_RETURN(ysError::YDS_NO_ERROR);
}

void ysDS8Device::ProcessAudioBuffers() {
	int bufferCount = m_audioBuffers.GetNumObjects();
	for(int i = bufferCount - 1; i >= 0; i--) {
		ysDS8AudioBuffer *ds8Buffer = static_cast<ysDS8AudioBuffer *>(m_audioBuffers.Get(i));

		if (ds8Buffer->GetBufferMode() == ysAudioBuffer::Mode::PlayOnce) {
			DWORD status;
			if (ds8Buffer->m_buffer->GetStatus(&status)) {
				if (!(status & DSBSTATUS_PLAYING)) {
					m_audioBuffers.Delete(ds8Buffer->GetIndex());
				}
			}
		}
	}
}
