#define _USE_MATH_DEFINES

#include "ToneGenerator.h"

#include <Windows.h>
#include <cmath>

#include <iostream>

    void ToneGenerator::setFormat(WAVEFORMATEX* wfex) {
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
    void ToneGenerator::fillBuffer(UINT32 bufferFrameCount, BYTE* pData, DWORD* flags, float frequency, float amp, double samplesPerSecond) {
        //const UINT16 formatTag = EXTRACT_WAVEFORMATEX_ID(&format.SubFormat);
        float* fData = (float*)pData;
        /*
        for (UINT32 i = 0; i < format.Format.nChannels * bufferFrameCount; i++) {
            fData[i] = 0;
        }
        */

        double theta = frequency * (2.0 * (double)M_PI) / samplesPerSecond;
        double ampl = 1;
        for (UINT32 i = 0; i < bufferFrameCount * format.Format.nChannels; i++) {
            fData[i] = (float)(ampl * amp * sin(theta * (double)i));
            ampl *= 0.999988;
        }
        /*
        if (formatTag == WAVE_FORMAT_IEEE_FLOAT) {
            
        }
        else if (formatTag == WAVE_FORMAT_PCM) {
            std::cout << "Wave PCM format!" << std::endl;
        }
        else {
            //memset(pData, 0, wfex.Format.nBlockAlign * bufferFrameCount);
            *flags = AUDCLNT_BUFFERFLAGS_SILENT;
        }
        */
    }