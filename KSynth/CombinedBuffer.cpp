#define _USE_MATH_DEFINES

#include "CombinedBuffer.h"

#include <Windows.h>
#include <cmath>

#include <iostream>
#include <thread>

void CombinedBuffer::setFormat(WAVEFORMATEX* wfex) {
    if (wfex->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        format = *reinterpret_cast<WAVEFORMATEXTENSIBLE*>(wfex);
    }
    else {
        format.Format = *wfex;
        format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        INIT_WAVEFORMATEX_GUID(&format.SubFormat, wfex->wFormatTag);
        format.Samples.wValidBitsPerSample = format.Format.wBitsPerSample;
        format.dwChannelMask = 0;
    }
}

// (The size of an audio frame = nChannels * wBitsPerSample)
void CombinedBuffer::fillBuffer(UINT32 buffSize, BYTE* pData, DWORD* flags) {
    //std::cout << "Filling buffer" << std::endl;

    //std::cout << "Filling buffer with " << (float)buffSize / (float)sizeof(float) / (float)channels / (float)sampleRate << " seconds of audio data." << std::endl;
    //std::cout << "Buffer filling position in seconds (should be behind): " << (float)bufferOutputPosition / (float)sizeof(float) / (float)channels / (float)sampleRate << std::endl;

    //std::cout << "Buffer time delta in seconds: " << (float)((bufferTimer->getCurrentMicroseconds() - currentBufferStartTime) / 1000000.0) - ((float)bufferOutputPosition / (float)sizeof(float) / (float)channels / (float)sampleRate) << std::endl;

    /*
    while ((bufferTimer->getCurrentMicroseconds() - currentBufferStartTime) / 1000000.0 - (float)bufferOutputPosition / (float)sizeof(float) / (float)channels / (float)sampleRate <= maxRenderBufferSeconds) {
        //std::cout << "sleeping for a bit" << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    */

    const UINT16 formatTag = EXTRACT_WAVEFORMATEX_ID(&format.SubFormat);

    //std::cout << "Filling buffer from index " << bufferOutputPosition << " to index " << bufferOutputPosition + buffSize << std::endl;
    if (formatTag == WAVE_FORMAT_IEEE_FLOAT) {
        //std::cout << "Start index: " << bufferOutputPosition << ", end index: " << bufferOutputPosition + buffSize << std::endl;
        for (unsigned int i = 0; i < buffSize; i++) {
            if (bufferOutputPosition >= bufferSize) {
                //std::cout << "Swiching buffers." << std::endl;

                /*
                for (unsigned int ii = 0; ii < bufferSize / sizeof(float); ii++) {
                    if (floatBuffer[0][ii] == 0) {
                        std::cout << "Buffer at index " << ii << ": " << floatBuffer[0][ii] << std::endl;
                    }
                }
                */

                /*
                for (unsigned int ii = 0; ii < bufferSize / sizeof(float); ii++) {
                    if (testMap.find(ii) != testMap.end()) {
                        std::cout << "Index " << ii << " size: " << testMap[ii] << std::endl;
                    }
                    std::cout << "Index " << ii << ": " << (float) floatBuffer[0][ii] << std::endl;
                }
                */

                mtx.lock();
                BYTE* firstBuffer = buffer[0];

                buffer[0] = buffer[1];
                delete[] firstBuffer;
                buffer[1] = buffer[2];
                buffer[2] = new BYTE[bufferSize];

                for (unsigned int ii = 0; ii < bufferSize; ii++) {
                    buffer[2][ii] = 0;
                }

                for (unsigned int ii = 0; ii < 3; ii++) {
                    floatBuffer[ii] = (float*)buffer[ii];
                }

                currentAddingBuffer = 0;
                bufferOutputPosition = 0;

                currentBufferStartTime += maxSampleSeconds * 1000000.0;

                totalFilledBuffers++;
                mtx.unlock();
            }

            pData[i] = buffer[0][bufferOutputPosition++];
        }

        float* fPData = (float*)pData;
        for (unsigned int i = 0; i < buffSize / sizeof(float); i++) {
            fPData[i] *= volume;
        }
    }
    else if (formatTag == WAVE_FORMAT_PCM) {
        std::cout << "Wave PCM format!" << std::endl;
    }
    else {
        //memset(pData, 0, wfex.Format.nBlockAlign * bufferFrameCount);
        std::cout << "Other audio format!" << std::endl;
        *flags = AUDCLNT_BUFFERFLAGS_SILENT;
    }
}

void CombinedBuffer::start() {
    bufferTimer.start();
}

void CombinedBuffer::addToBuffer(BYTE* buff, unsigned int startFrame, unsigned int bufferFrames, float amplitude, float volRelease) { //REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec
    //bufferPosition = max(((bufferTimer->getCurrentMicroseconds() - currentBufferStartTime) / 1000000.0) * sampleRate * channels, 0.0);
    totalCurrentAddingBuffer = floor(startFrame * format.Format.nChannels * sizeof(float) / (float)bufferSize);
    currentAddingBuffer = (unsigned int) max((int)(totalCurrentAddingBuffer - totalFilledBuffers), 0);
    bufferPosition = startFrame * format.Format.nChannels - bufferSize / sizeof(float) * totalCurrentAddingBuffer;

    /*
    while(pitchTimes[currentPitchIndex] < startFrame / format.Format.nSamplesPerSec) {
        currentPitchIndex++;
    }
    */

    /*
    currentAddingBuffer = 0;
    while (bufferPosition > bufferSize / sizeof(float)) {
        currentAddingBuffer++;
        bufferPosition -= bufferSize / sizeof(float);
    }
    */

    //std::cout << "Buffer position in seconds (should be ahead): " << (float)bufferPosition / (float)channels / (float)sampleRate << std::endl;

    float* fData = (float*)buff;

    unsigned int ii = 0;
    //std::cout << "Buffer frames: " << bufferFrames << ", Expected size in floats: " << bufferFrames * format.Format.nChannels << std::endl;
    //std::cout << "Buffer position: " << bufferPosition << std::endl;
    //std::cout << "Buffer size: " << bufferSize << ", Buffer size in floats: " << bufferSize / sizeof(float) << std::endl;

    //Debug things
    //std::cout << "Total filled buffers: " << (unsigned int)totalFilledBuffers << std::endl;
    //std::cout << "Total adding buffer: " << (unsigned int)totalCurrentAddingBuffer << std::endl;
    //std::cout << "Current adding buffer: " << (unsigned int)currentAddingBuffer << std::endl;
    //std::cout << "Buffer position: " << (unsigned int)bufferPosition << std::endl;
    mtx.lock();
    //std::cout << "Adding to buffer from index " << (bufferPosition + 0 - ii * (bufferSize / sizeof(float))) * sizeof(float) << " to index " << (bufferPosition + bufferFrames * format.Format.nChannels - ii * (bufferSize / sizeof(float))) * sizeof(float) << std::endl;
    /*
    double pitchBendFrequency;
    double noteFrequency;
    float pitchBendFactor = 1;
    if (pitchData[currentPitchIndex] != 8192) { //If the pitch data is not the normal pitch bend value
        pitchBendFrequency = 16.35 * pow(pow(2.0, 1.0 / 12.0), (double)key + (((double)pitchData[currentPitchIndex] - 8192) / 8192.0) * 127.0f);
        noteFrequency = 16.35 * pow(pow(2.0, 1.0 / 12.0), (double)key);
        pitchBendFactor = noteFrequency / pitchBendFrequency;
    }
    */
    for (unsigned int i = 0; i < bufferFrames * format.Format.nChannels; i++) {
        if (bufferPosition + i - ii * (bufferSize / sizeof(float)) >= bufferSize / sizeof(float)) {
            ii++;
        }
        //std::cout << "Writing index " << i << std::endl;

        //std::cout << "Adding thing to buffer: " << fData[i] * amplitude << std::endl;

        if (bufferFrames * format.Format.nChannels - i <= volRelease) {
            //std::cout << "Test thing: " << (1.0f - (i - volRelease) / (bufferFrames * format.Format.nChannels - volRelease)) << std::endl;
            floatBuffer[currentAddingBuffer + ii][bufferPosition + i - ii * (bufferSize / sizeof(float))] += fData[i] * amplitude * (1.0f - (i - (bufferFrames * format.Format.nChannels - volRelease)) / volRelease);
        }
        else {
            floatBuffer[currentAddingBuffer + ii][bufferPosition + i - ii * (bufferSize / sizeof(float))] += fData[i] * amplitude; //rare access violation here, maybe look into it if it is persistant (happened after a few minutes of playing TTB F1.mid)
        }
    }

    mtx.unlock();
}
/*
void CombinedBuffer::addToBuffer(BYTE* buff, unsigned int startIndex, unsigned int size, float amplitude) { //REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec
    //std::cout << "Start sample index: " << startIndex << ", end index: " << startIndex + size << std::endl;

    currentAddingBuffer = 0;
    while (bufferPosition > bufferSize / sizeof(float)) {
        //std::cout << "Doing thing with buffer position because too big." << std::endl;
        currentAddingBuffer++;
        bufferPosition -= bufferSize / sizeof(float);
    }

    //std::cout << "Buffer position in seconds (should be ahead): " << (float)bufferPosition / (float)channels / (float)sampleRate << std::endl;
    
    float* fData = (float*)buff;

    unsigned int ii = 0;
    //std::cout << "Buffer frames: " << bufferFrames << ", Expected size in floats: " << bufferFrames * format.Format.nChannels << std::endl;
    //std::cout << "Buffer position: " << bufferPosition << std::endl;
    //std::cout << "Buffer size: " << bufferSize << ", Buffer size in floats: " << bufferSize / sizeof(float) << std::endl;
    //mtx.lock();
    //std::cout << "Adding to buffer from index " << (bufferPosition + 0 - ii * (bufferSize / sizeof(float))) * sizeof(float) << " to index " << (bufferPosition + bufferFrames * format.Format.nChannels - ii * (bufferSize / sizeof(float))) * sizeof(float) << std::endl;
    //std::cout << "Currently adding in buffer " << currentAddingBuffer + ii << std::endl;
    for (unsigned int i = startIndex * format.Format.nChannels; i < (startIndex + size) * format.Format.nChannels; i++) {
        if (bufferPosition + (i - startIndex * format.Format.nChannels) - ii * (bufferSize / sizeof(float)) >= bufferSize / sizeof(float)) {
            //std::cout << "Buffer position increasing at index " << i << std::endl;
            ii++;

            //std::cout << "Currently adding in buffer " << currentAddingBuffer + ii << std::endl;
        }

        floatBuffer[currentAddingBuffer + ii][bufferPosition + (i - startIndex * format.Format.nChannels) - ii * (bufferSize / sizeof(float))] += fData[i] * amplitude;
    }

    //mtx.unlock();
}
*/

unsigned int CombinedBuffer::updateBufferPosition() {
    //std::cout << "Updating buffer position." << std::endl;

    unsigned int pos = ((bufferTimer.getCurrentMicroseconds() - currentBufferStartTime) / 1000000.0) * sampleRate * channels;
    bufferPosition = max(floor(pos / 2.0) * 2, 0);

    return bufferPosition;
}

unsigned int CombinedBuffer::addToBufferPosition(unsigned int bufferFloats) {
    //std::cout << "Updating buffer position." << std::endl;

    bufferPosition += bufferFloats;

    return bufferPosition;
}

CombinedBuffer::CombinedBuffer(unsigned int maxSampleSeconds, unsigned int sampleRate, unsigned int channels, float maxRenderBufferSeconds) : format() {
    this->maxSampleSeconds = maxSampleSeconds;
    this->sampleRate = sampleRate;
    this->channels = channels;
    this->maxRenderBufferSeconds = maxRenderBufferSeconds;

    bufferSize = maxSampleSeconds * sampleRate * channels * sizeof(float);
    buffer = new BYTE*[3]; //Buffer 0 for holding note starts, buffer 1 for holding note tails of buffer 0 and for holding a few note starts while waiting for WasapiRenderer, buffer 2 for holding note tails of buffer 1
    for (unsigned int i = 0; i < 3; i++) {
        buffer[i] = new BYTE[bufferSize];
        for (unsigned int ii = 0; ii < bufferSize; ii++) {
            buffer[i][ii] = 0;
        }
    }

    floatBuffer = new float*[3];
    for (unsigned int i = 0; i < 3; i++) {
        floatBuffer[i] = (float*)buffer[i];
    }
}