#include "../include/yds_windows_audio_wave_file.h"

#include <string>

ysWindowsAudioWaveFile::ysWindowsAudioWaveFile() : ysAudioFile(Platform::Windows, AudioFormat::Wave) {
    m_fileHandle = NULL;
    m_dataSegmentOffset = 0;
}

ysWindowsAudioWaveFile::~ysWindowsAudioWaveFile() {
    DestroyInternalBuffer();
    if (m_fileHandle) mmioClose(m_fileHandle, 0);
}

ysAudioFile::Error ysWindowsAudioWaveFile::OpenFile(const char *fname) {
    ysAudioFile::Error error = ysAudioFile::OpenFile(fname);
    if (error != Error::None) return error;

    char localFname[256];
    strcpy_s(localFname, 256, fname);

    // Attempt to open the file
    HMMIO file = mmioOpen(localFname, 0, MMIO_READ);

    if (file == NULL) {
        return Error::CouldNotOpenFile;
    }

    // Check that the file is a wave file
    MMCKINFO mmckinfoParent;
    MMCKINFO mmckinfoSubchunk;

    mmckinfoParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(file, (LPMMCKINFO)&mmckinfoParent, NULL, MMIO_FINDRIFF)) {
        // Not a waveform file
        mmioClose(file, 0);
        return Error::InvalidFileFormat;
    }

    // Find the wave format segment
    mmckinfoSubchunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(file, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK)) {
        // Could not find format segment
        mmioClose(file, 0);
        return Error::InvalidFileFormat;
    }

    DWORD dwFmtSize = mmckinfoSubchunk.cksize;
    WAVEFORMATEX format;

    // Read the "FMT" chunk. 
    if (mmioRead(file, (HPSTR)(&format), dwFmtSize) != dwFmtSize) {
        // Failed to read format chunk
        mmioClose(file, 0);
        return Error::InvalidFileFormat;
    }

    // Find the data segment
    mmckinfoSubchunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(file, &mmckinfoSubchunk, &mmckinfoParent, MMIO_FINDCHUNK)) {
        // No data chunk
        mmioClose(file, 0);
        return Error::InvalidFileFormat;
    }

    // Get size of data segment
    DWORD dwDataSize = mmckinfoSubchunk.cksize;
    if (dwDataSize == 0L) {
        // File contains no data
        mmioClose(file, 0);
        return Error::InvalidFileFormat;
    }

    // Save the location within the file
    long fileOffset = mmioSeek(file, 0, SEEK_CUR);
    if (fileOffset < 0) {
        // Weird error
        mmioClose(file, 0);
        return Error::InvalidFileFormat;
    }

    // Everything worked, commit data

    m_fileHandle = file;
    m_dataSegmentOffset = fileOffset;

    m_audioParameters.m_channelCount = format.nChannels;
    m_audioParameters.m_sampleRate = format.nSamplesPerSec;
    m_audioParameters.m_bitsPerSample = (format.nBlockAlign / format.nChannels) * 8;

    m_sampleCount = dwDataSize / format.nBlockAlign;

    m_fileOpen = true;

    return Error::None;
}

ysAudioFile::Error ysWindowsAudioWaveFile::CloseFile() {
    Error error = ysAudioFile::CloseFile();
    if (error != Error::None) return error;

    mmioClose(m_fileHandle, 0);

    return Error::None;
}

ysAudioFile::Error ysWindowsAudioWaveFile::GenericRead(SampleOffset offset, SampleOffset size, void *buffer) {
    Error error = ysAudioFile::GenericRead(offset, size, buffer);
    if (error != Error::None) return error;

    long fileOffset = m_audioParameters.GetSizeFromSamples(offset) + m_dataSegmentOffset;
    long fileSize = (long)m_audioParameters.GetSizeFromSamples(size);

    mmioSeek(m_fileHandle, fileOffset, SEEK_SET);

    // Read the waveform-audio data subchunk. 
    if (mmioRead(m_fileHandle, (HPSTR)buffer, (DWORD)fileSize) != (DWORD)fileSize) {
        // Failed
        return Error::FileReadError;
    }

    return Error::None;
}
