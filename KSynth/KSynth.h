#pragma once
#include <wtypes.h>
#include "WasapiRenderer.h"
#include "CombinedBuffer.h"
#include "KSFL/SoundFontFile.h"
#include <thread>

class KSynth {
private:
	//uint16_t** pitchAutomationData = new uint16_t*[16];
	//uint64_t** pitchAutomationTimes = new uint64_t*[16];

	WasapiRenderer* renderer = new WasapiRenderer();

	float* keyFrequencyTable = new float[128];
	unsigned int keySampleSize;
	BYTE** keySamples = new BYTE * [128];

	std::thread noteBufferThread;
	std::thread renderThread;

	//void bufferLoop();

	void startAudio();

public:
	unsigned int audioDelay;

	CombinedBuffer* combinedBuffer;

	SoundFontFile* soundFont;

	KSynth() {}

	void initialize(std::string sfPath);

	unsigned int getLengthInDataPoints(uint64_t startMicroseconds, uint64_t endMicroseconds, float releaseFactor);

	unsigned int sendNote(BYTE channel, BYTE key, BYTE velocity, uint64_t startMicroseconds, uint64_t endMicroseconds, float releaseFactor);

	void sendPitchAutomationData(uint8_t channel, uint16_t* pitchData, uint64_t* pitchTimes);

	void setVolume(float volume);
};