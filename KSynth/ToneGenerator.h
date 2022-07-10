#pragma once

#include <Audiopolicy.h>

class ToneGenerator {
public:
    ToneGenerator() : format() {}

    void setFormat(WAVEFORMATEX* wfex);

    // (The size of an audio frame = nChannels * wBitsPerSample)
    void fillBuffer(UINT32 bufferFrameCount, BYTE* pData, DWORD* flags, float frequency, float amp, double samplesPerSecond);

private:
    WAVEFORMATEXTENSIBLE format;
};