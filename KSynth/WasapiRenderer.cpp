#include "WasapiRenderer.h"

// WASAPI
#include <Audiopolicy.h>
#include <Audioclient.h>
#include <iostream>

HRESULT WasapiRenderer::initialize() {
    ToneGenerator* toneGenerator = nullptr;

    HRESULT hr = CoInitialize(NULL);
    EXIT_ON_ERROR(hr);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL,
        CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    EXIT_ON_ERROR(hr);

    hr = pEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr);

    hr = pDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    EXIT_ON_ERROR(hr);

    PropVariantInit(&name);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &name);
    EXIT_ON_ERROR(hr);

    hr = pDevice->Activate(__uuidof(pAudioClient), CLSCTX_ALL,
        NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr);

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
        0, hnsRequestedDuration,
        0, pwfx, NULL);
    EXIT_ON_ERROR(hr);
    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr);
    hr = pAudioClient->GetService(IID_PPV_ARGS(&pRenderClient));
    EXIT_ON_ERROR(hr);

    /*
    playingNotes = new int**[threadCount];
    //for (unsigned int iiii = 0; iiii < threadCount; iiii++) {
        for (unsigned int i = 0; i < 16; i++) {
            playingNotes[i] = new int* [128];
            for (unsigned int ii = 0; ii < 128; ii++) {
                playingNotes[i][ii] = new int[128];
                for (unsigned int iii = 0; iii < 128; iii++) {
                    playingNotes[i][ii][iii] = -1;
                }
            }
        }
    //}
    */

    return hr;

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pRenderClient);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pPropertyStore); // you forgot to free the property store
    SAFE_RELEASE(pEnumerator);
    return hr;
}

HRESULT WasapiRenderer::playAudioStream(CombinedBuffer* pMySource) {
    //int currentMicroseconds = 0;
    //int lastMicroseconds = 0;
    //int bufferPosition = 0;
    //int lastBufferPosition = 0;
    unsigned int c;
    unsigned int k;
    unsigned int v;

    // Tell the audio source which format to use.
    pMySource->setFormat(pwfx);

    // Grab the entire buffer for the initial fill operation.
    HRESULT hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    EXIT_ON_ERROR(hr);

    // Load the initial data into the shared buffer.
    pMySource->fillBuffer(bufferFrameCount * pwfx->nChannels * sizeof(float), pData, &flags);

    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    EXIT_ON_ERROR(hr);

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;
    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr);
    // Each loop fills about half of the shared buffer.
    //sleepTimer.start();

    //lastBufferPosition = 0;
    pMySource->bufferPosition = 1 * pwfx->nSamplesPerSec * pwfx->nChannels;
    pMySource->start();
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT) {
        //std::cout << "Playing thing" << std::endl;

        Sleep(hnsActualDuration / (float)REFTIMES_PER_MILLISEC / 2.0);

        /*
        startSleepTime = sleepTimer.getCurrentMicroseconds();

        while ((sleepTimer.getCurrentMicroseconds() - startSleepTime) / 1000000.0 <= (float)hnsActualDuration / (float)REFTIMES_PER_SEC / 2.0) {
            //std::cout << "Doing buffer creation loop" << std::endl;

            //currentMicroseconds = (sleepTimer.getCurrentMicroseconds() - startSleepTime);
            bufferPosition = pMySource->updateBufferPosition();

            if (bufferPosition < lastBufferPosition) {
                std::cout << "Updating last buffer position!" << std::endl;
                lastBufferPosition -= ((int)pMySource->bufferSize) / (int)sizeof(float);
            }

            for (unsigned int c = 0; c < 16; c++) {
                for (unsigned int k = 0; k < 128; k++) {
                    for (unsigned int v = 0; v < 128; v++) {
                        if (playingNotes[c][k][v] != -1 && playingNotes[c][k][v] < 10 * pwfx->nSamplesPerSec) { //sample length is 10
                            //std::cout << "s: " << ((float)keySampleSize / (float)sizeof(float) / (float)renderer->pwfx->nChannels / 40.0) << std::endl;
                            //pwfx->nSamplesPerSec * 10 * pwfx->nChannels * sizeof(float)

                            //std::cout << "Buffer position: " << bufferPosition << ", last buffer position: " << lastBufferPosition << ", size of buffer: " << (bufferPosition - lastBufferPosition) << std::endl;
                            pMySource->addToBuffer(keySamples[k], playingNotes[c][k][v] / pwfx->nChannels, (bufferPosition - lastBufferPosition) / pwfx->nChannels, v / 127.0);

                            playingNotes[c][k][v] += bufferPosition - lastBufferPosition;
                            //playingNotes[c][k][v] = 0;
                        }
                        else {
                            playingNotes[c][k][v] = -1; //if more than or equal to 10 then kill voice
                        }
                    }
                }
            }

            lastBufferPosition = bufferPosition;
        }
        */

        //std::cout << "Rendering stuff." << std::endl;

        //std::cout << "Putting some things in the buffer." << std::endl;
        //std::cout << "Actual duration: " << hnsActualDuration << std::endl;
        // See how much buffer space is available.
        UINT32 numFramesPadding;
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr);

        UINT32 numFramesAvailable = bufferFrameCount - numFramesPadding;
        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr);

        /*
        if (numFramesAvailable > 0) {
            //for (t = 0; t < threadCount; t++) {
            if (polyphony < maxPolyphony) {
                velocityGate = max((int) velocityGate - 1, 1);
            }
            polyphony = 0;
                for (c = 0; c < 16; c++) {
                    for (k = 0; k < 128; k++) {
                        for (v = 127; v > velocityGate; v--) {
                            if (playingNotes[c][k][v] != -1 && playingNotes[c][k][v] < 10 * pwfx->nSamplesPerSec) { //sample length is 10
                                if (polyphony + 1 > maxPolyphony) {
                                    velocityGate = min(velocityGate + 1, 127);

                                    break;
                                }

                                //std::cout << "s: " << ((float)keySampleSize / (float)sizeof(float) / (float)renderer->pwfx->nChannels / 40.0) << std::endl;
                                //pwfx->nSamplesPerSec * 10 * pwfx->nChannels * sizeof(float)

                                //std::cout << "Buffer position: " << bufferPosition << ", last buffer position: " << lastBufferPosition << ", size of buffer: " << (bufferPosition - lastBufferPosition) << std::endl;
                                pMySource->addToBuffer(keySamples[k], playingNotes[c][k][v], numFramesAvailable, v / 127.0);

                                playingNotes[c][k][v] += numFramesAvailable;
                                //playingNotes[c][k][v] = 0;

                                polyphony++;
                            }
                            else {
                                playingNotes[c][k][v] = -1; //if more than or equal to 10 then kill voice
                            }
                        }
                    }
                }
            //}

            pMySource->addToBufferPosition(numFramesAvailable * pwfx->nChannels);
            //pMySource->updateBufferPosition(); //make it so the buffer position goes off of wasapi timing, not timer timing
        }
        */





        // Get next 1/2-second of data from the audio source.
        pMySource->fillBuffer(numFramesAvailable * pwfx->nChannels * sizeof(float), pData, &flags);

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr);
    }
    // Wait for last data in buffer to play before stopping.
    Sleep((hnsActualDuration / REFTIMES_PER_MILLISEC / 2));
    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr);

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pRenderClient);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pPropertyStore); // you forgot to free the property store
    SAFE_RELEASE(pEnumerator);
    return hr;
}