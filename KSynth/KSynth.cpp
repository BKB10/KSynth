#include "KSynth.h"
#include <iostream>

void KSynth::initialize(std::string sfPath) {
	soundFont = new SoundFontFile(sfPath); //FluidR3 GM Modified Just Piano.sf2

	renderer->initialize();

	audioDelay = renderer->hnsRequestedDuration / REFTIMES_PER_MILLISEC;

	/*
	for (unsigned int i = 0; i < 128; i++) {
		keyFrequencyTable[i] = 16.35 * pow(pow(2.0, 1.0 / 12.0), (double)i);
	}

	ToneGenerator* toneGenerator = new ToneGenerator();
	toneGenerator->setFormat(renderer->pwfx);
	keySampleSize = renderer->pwfx->nSamplesPerSec * 10 * renderer->pwfx->nChannels * sizeof(float); //10 second buffer
	for (unsigned int i = 0; i < 128; i++) {
		keySamples[i] = new BYTE[keySampleSize];
		toneGenerator->fillBuffer(keySampleSize / sizeof(float) / renderer->pwfx->nChannels, keySamples[i], 0, keyFrequencyTable[i], 0.01, renderer->pwfx->nSamplesPerSec);
	}
	*/

	keySampleSize = renderer->pwfx->nSamplesPerSec * 10 * renderer->pwfx->nChannels * sizeof(float); //10 second buffer

	/*
	ToneGenerator* toneGenerator = new ToneGenerator();
	toneGenerator->setFormat(renderer->pwfx);
	keySamples[60] = new BYTE[keySampleSize];
	toneGenerator->fillBuffer(keySampleSize / sizeof(float) / renderer->pwfx->nChannels, keySamples[60], 0, 16.35 * pow(pow(2.0, 1.0 / 12.0), 60.0), 0.01, renderer->pwfx->nSamplesPerSec);
	*/

	for (unsigned int i = 0; i < 128; i++) {
		keySamples[i] = new BYTE[keySampleSize];
		float* floatData = (float*)keySamples[i];
		float* sampleData = soundFont->samples[0][i].buffer;

		/*
		for (unsigned int ii = 0; ii < soundFont->samples[0][i].bufferSize; ii += 2) {
			sampleData[ii + 1] = 0.0f;
		}
		*/

		std::cout << "Key: " << i << ", renderer sample rate: " << renderer->pwfx->nSamplesPerSec << ", sample sample rate: " << soundFont->samples[0][i].sampleRate << std::endl;
		for (unsigned int ii = 0; ii < min(soundFont->samples[0][i].bufferSize / 2, keySampleSize / 4 / 2); ii++) { 
			//floatData[ii] = sampleData[(unsigned int)min((float)ii * ((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), soundFont->samples[0][i].bufferSize)];
			//floatData[ii] = sampleData[ii];

			if (renderer->pwfx->nSamplesPerSec < soundFont->samples[0][i].sampleRate) { //todo: DO THISSS!!!!!!!!!!!!
				//std::cout << "Case 1: Putting sample data from index " << ii << " to " << (unsigned int)min((float)ii * (float)((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), (float)keySampleSize / 4) << std::endl;
				//floatData[ii] = sampleData[(unsigned int)min((float)ii * (float)((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), (float)soundFont->samples[0][i].bufferSize)];
				//floatData[ii * 2] = sampleData[(unsigned int)min((unsigned int)(ii * (float)((float)soundFont->samples[0][i].sampleRate / (float)renderer->pwfx->nSamplesPerSec)) * 2, soundFont->samples[0][i].bufferSize)];
				//floatData[ii * 2 + 1] = sampleData[(unsigned int)min((unsigned int)(ii * (float)((float)soundFont->samples[0][i].sampleRate / (float)renderer->pwfx->nSamplesPerSec)) * 2 + 1, soundFont->samples[0][i].bufferSize)];
			}
			else {
				//std::cout << "Case 2: Putting sample data from index " << (unsigned int)min((float)ii * (float)((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), (float)soundFont->samples[0][i].bufferSize) << " to " << ii << std::endl;
				floatData[ii * 2] = sampleData[(unsigned int)min((unsigned int) (ii * (float)((float)soundFont->samples[0][i].sampleRate / (float)renderer->pwfx->nSamplesPerSec)) * 2, soundFont->samples[0][i].bufferSize)];
				floatData[ii * 2 + 1] = sampleData[(unsigned int)min((unsigned int) (ii * (float)((float)soundFont->samples[0][i].sampleRate / (float)renderer->pwfx->nSamplesPerSec)) * 2 + 1, soundFont->samples[0][i].bufferSize)];
			}

			/*
			for (int iii = (unsigned int)min((float)ii * ((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), soundFont->samples[0][i].bufferSize); iii < (unsigned int)min((float)(ii + 1) * ((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), soundFont->samples[0][i].bufferSize); iii++) {
				floatData[iii] = sampleData[(unsigned int)min((float)ii * ((float)renderer->pwfx->nSamplesPerSec / (float)soundFont->samples[0][i].sampleRate), soundFont->samples[0][i].bufferSize)];
			}
			*/
		}
	}

	/*
	float* floatData = (float*)keySamples[26];
	float* sampleData = soundFont->samples[0][26].buffer;
	for (unsigned int i = 0; i < 44100; i++) {
		if (i % 50 == 0) {
			std::cout << "Index " << i << ": Original (" << soundFont->samples[0][26].sampleRate << "): " << sampleData[i] << ", New (" << renderer->pwfx->nSamplesPerSec << "): " << floatData[i] << std::endl;
		}
	}
	*/

	combinedBuffer = new CombinedBuffer(60 * 2, renderer->pwfx->nSamplesPerSec, renderer->pwfx->nChannels, renderer->hnsRequestedDuration / (float)REFTIMES_PER_SEC); //There are 10000000 reference units in a second and the variable is in reference units
	//combinedBuffer->start();

	//noteBufferThread = std::thread(&KSynth::bufferLoop, this);
	//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	renderThread = std::thread(&KSynth::startAudio, this);
}

void KSynth::startAudio() {
	renderer->playAudioStream(combinedBuffer);
}

/*
void KSynth::bufferLoop() {
	while (true) {
		for (unsigned int c = 0; c < 16; c++) {
			for (unsigned int k = 0; k < 128; k++) {
				if (renderer->playingNotes[c][k] != 0) {
					//std::cout << "Sending with size: " << ((float)keySampleSize / (float)sizeof(float) / (float)renderer->pwfx->nChannels / 40.0) << std::endl;
					combinedBuffer->addToBuffer(keySamples[k], ((float)keySampleSize / (float)sizeof(float) / (float)renderer->pwfx->nChannels / 10.0), playingNotes[c][k] / 127.0);
					playingNotes[c][k] = 0;
				}
			}
		}
	}
}
*/

unsigned int KSynth::getLengthInDataPoints(uint64_t startMicroseconds, uint64_t durationMicroseconds, float releaseFactor) {
	return min((double)durationMicroseconds / 1000000.0 * renderer->pwfx->nSamplesPerSec + (0.5 * releaseFactor * renderer->pwfx->nSamplesPerSec), keySampleSize / renderer->pwfx->nChannels / sizeof(float));
}

unsigned int KSynth::sendNote(BYTE channel, BYTE key, BYTE velocity, uint64_t startMicroseconds, uint64_t durationMicroseconds, float releaseFactor) {
	//startMicroseconds / 1000000.0 * renderer->pwfx->nSamplesPerSec
	//std::cout << "Release: " << soundFont->samples[0][key].release << std::endl;
	combinedBuffer->addToBuffer(keySamples[key], startMicroseconds / 1000000.0 * renderer->pwfx->nSamplesPerSec, min((double)durationMicroseconds / 1000000.0 * renderer->pwfx->nSamplesPerSec + (0.5 * releaseFactor * renderer->pwfx->nSamplesPerSec), keySampleSize / renderer->pwfx->nChannels / sizeof(float)), (double) velocity / 127.0, 0.5 * releaseFactor * renderer->pwfx->nSamplesPerSec * renderer->pwfx->nChannels); //currently hardcoded fadeout of 0.5 used to be: soundFont->samples[0][key].release

	return getLengthInDataPoints(startMicroseconds, durationMicroseconds, releaseFactor);
}

/*
void KSynth::sendPitchAutomationData(uint8_t channel, uint16_t* pitchData, uint64_t* pitchTimes) {
	pitchAutomationData[channel] = pitchData;
	pitchAutomationTimes[channel] = pitchTimes;
}
*/

void KSynth::setVolume(float volume) {
	combinedBuffer->volume = volume;
}