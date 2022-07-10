#pragma once
#include <audiopolicy.h>
#include "Timer.h"
#include <queue>
#include <mutex>
#include <map>

class CombinedBuffer {
public:
    CombinedBuffer(unsigned int maxSampleSeconds, unsigned int sampleRate, unsigned int channels, float maxRenderBufferSeconds);

    void setFormat(WAVEFORMATEX* wfex);

    void start();

    void fillBuffer(UINT32 bufferFrameCount, BYTE* pData, DWORD* flags);

    //void addToBuffer(BYTE* buff, unsigned int buffSize, float amplitude);

    void addToBuffer(BYTE* buff, unsigned int startFrame, unsigned int bufferFrames, float amplitude, float volRelease);

    unsigned int updateBufferPosition();

    unsigned int addToBufferPosition(unsigned int bufferFloats);

    unsigned int bufferSize;
    unsigned int bufferPosition = 0;
    unsigned int currentAddingBuffer = 0;
    unsigned int totalCurrentAddingBuffer = 0;
    unsigned int totalFilledBuffers = 0;

    unsigned int maxSampleSeconds;

    Timer bufferTimer;

    float volume = 1;

private:
    unsigned int lastThing = 0;

    WAVEFORMATEXTENSIBLE format;

    BYTE** buffer;
    float** floatBuffer;
    unsigned int bufferOutputPosition = 0;
    std::mutex mtx;

    unsigned int sampleRate;
    unsigned int channels;
    float maxRenderBufferSeconds;

    uint32_t currentBufferStartTime = 0;
};