#pragma once

// Windows multimedia device
#include <Mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "ToneGenerator.h"
#include "CombinedBuffer.h"

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000ll
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

/*
struct RenderingNote {
public:
    float currentSeconds;
};
*/

class WasapiRenderer {
public:
    //unsigned int maxPolyphony = 20;
    //unsigned int polyphony = 0;
    //unsigned int velocityGate = 1;

    //int*** playingNotes;
    //unsigned int startSleepTime;
    //Timer sleepTimer;

    //unsigned int threadCount;

    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_MILLISEC * 1000 * 5; // / 1000 * 20
    REFERENCE_TIME hnsActualDuration;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IPropertyStore* pPropertyStore = NULL;
    IMMDevice* pDevice = NULL;
    IAudioClient* pAudioClient = NULL;
    IAudioRenderClient* pRenderClient = NULL;
    WAVEFORMATEX* pwfx = NULL;
    UINT32 bufferFrameCount;
    BYTE* pData;
    DWORD flags = 0;
    PROPVARIANT name;

    WasapiRenderer() {}

    HRESULT initialize();

    HRESULT playAudioStream(CombinedBuffer* pMySource);
};