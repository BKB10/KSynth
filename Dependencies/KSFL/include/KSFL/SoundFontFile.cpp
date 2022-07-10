#include "SoundFontFile.h"
#include <iostream>
#include "wtypes.h"
#include <thread>

SoundFontFile::SoundFontFile() {

}

SoundFontFile::~SoundFontFile() {
	
}

SoundFontFile::SoundFontFile(std::string filePath) {
	load(filePath);
}

void SoundFontFile::skipChunk() {
	reader.skipNextBytes(*(uint32_t*)reader.getNextBytes(4));
}

void SoundFontFile::parseInfoChunk(uint32_t endByte) {
	while (reader.currentByte < endByte) {
		uint32_t chunkType = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));

		switch (chunkType) {
		case 'ifil':
			std::cout << "Version chunk!" << std::endl;
			skipChunk();
			break;
		case 'isng':
			std::cout << "Sound engine chunk!" << std::endl;
			skipChunk();
			break;
		case 'INAM':
			std::cout << "Sound bank name chunk!" << std::endl;
			skipChunk();
			break;
		case 'irom':
			std::cout << "Sound ROM name chunk!" << std::endl;
			skipChunk();
			break;
		case 'iver':
			std::cout << "Sound ROM version chunk!" << std::endl;
			skipChunk();
			break;
		case 'ICRD':
			std::cout << "Bank creation date chunk!" << std::endl;
			skipChunk();
			break;
		case 'IENG':
			std::cout << "Sound engineers chunk!" << std::endl;
			skipChunk();
			break;
		case 'IPRD':
			std::cout << "Product chunk!" << std::endl;
			skipChunk();
			break;
		case 'ICOP':
			std::cout << "Copyright chunk!" << std::endl;
			skipChunk();
			break;
		case 'ICMT':
			std::cout << "Comment chunk!" << std::endl;
			skipChunk();
			break;
		case 'ISFT':
			std::cout << "Editor software chunk!" << std::endl;
			skipChunk();
			break;
		default:
			std::cout << "Unknown chunk!" << std::endl;
			skipChunk();
			break;
		}
	}
}

void SoundFontFile::parseSampleChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	rawAudioData = reader.getNextBytes(chunkLength);
}

void SoundFontFile::parseSampleHeaderChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		reader.skipNextBytes(20);
		uint32_t startIndex = *(uint32_t*)reader.getNextBytes(4);
		uint32_t endIndex = *(uint32_t*)reader.getNextBytes(4);
		uint32_t startLoopIndex = *(uint32_t*)reader.getNextBytes(4);
		uint32_t endLoopIndex = *(uint32_t*)reader.getNextBytes(4);
		uint32_t sampleRate = *(uint32_t*)reader.getNextBytes(4);
		std::cout << "Sample rate: " << sampleRate << std::endl;
		uint8_t key = *reader.getNextBytes(1);
		//std::cout << "Key: " << (unsigned int)key << std::endl;
		reader.skipNextBytes(1);
		uint16_t link = *(uint16_t*) reader.getNextBytes(2);
		uint16_t type = *(uint16_t*) reader.getNextBytes(2);

		sampleHeaders.push_back(SoundFontSampleHeader { startIndex, endIndex, startLoopIndex, endLoopIndex, sampleRate, key, link, type });
	}
}

void SoundFontFile::parseInstrumentGenChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		uint16_t genOperation = *(uint16_t*)reader.getNextBytes(2);

		uint16_t data = *(uint16_t*) reader.getNextBytes(2);

		//std::cout << "Operation: " << (unsigned int)genOperation << ", data: " << (unsigned int) data << std::endl;

		instrumentGenerators.push_back(SoundFontGenerator{ genOperation, data });
	}
}

void SoundFontFile::parseInstrumentChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		reader.skipNextBytes(20);
		uint16_t instrumentBag = *(uint16_t*)reader.getNextBytes(2);

		instrumentIndexes.push_back(instrumentBag);

		std::cout << "Instrument bag index: " << instrumentBag << std::endl;
	}
}

void SoundFontFile::parseInstrumentBagChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		uint16_t gen = *(uint16_t*)reader.getNextBytes(2);
		uint16_t mod = *(uint16_t*)reader.getNextBytes(2);

		//std::cout << "Generator index: " << (unsigned int) gen << std::endl;

		instrumentBags.push_back(SoundFontInstrumentBag{ gen, mod });
	}
}

void SoundFontFile::parsePresetHeaderChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		reader.skipNextBytes(20);
		uint16_t preset = *(uint16_t*)reader.getNextBytes(2);
		uint16_t bank = *(uint16_t*)reader.getNextBytes(2);
		uint16_t presetBag = *(uint16_t*)reader.getNextBytes(2);
		reader.skipNextBytes(4 * 3);

		//std::cout << "Preset bag index: " << presetBag << std::endl;

		presetHeaders.push_back(SoundFontPresetHeader { preset, bank, presetBag });
	}
}

void SoundFontFile::parsePresetBagChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		uint16_t gen = *(uint16_t*)reader.getNextBytes(2);
		uint16_t mod = *(uint16_t*)reader.getNextBytes(2);

		//std::cout << "Gen index: " << gen << std::endl;

		presetBags.push_back(SoundFontPresetBag{ gen, mod });
	}
}

void SoundFontFile::parsePresetGenChunk() {
	uint32_t chunkLength = *(uint32_t*)reader.getNextBytes(4);
	uint32_t endByte = reader.currentByte + chunkLength;

	while (reader.currentByte < endByte) {
		uint16_t genOperation = *(uint16_t*)reader.getNextBytes(2);

		//std::cout << "Operation: " << (unsigned int)genOperation << std::endl;

		uint16_t data = *(uint16_t*)reader.getNextBytes(2);

		presetGenerators.push_back(SoundFontGenerator{genOperation, data});

		/*
		switch (genOperation) {
		case GEN_RELEASE_VOL_ENV:
		{
			int16_t data = *reader.getNextBytes(2);
			std::cout << "Release volume envolope: " << (unsigned int)data << std::endl;
			break;
		}

		case GEN_INSTRUMENT:
		{
			uint16_t data = *reader.getNextBytes(2);
			std::cout << "Instrument: " << (unsigned int)data << std::endl;
			break;
		}

		case GEN_KEY_RANGE:
		{
			uint8_t min = *reader.getNextBytes(1);
			uint8_t max = *reader.getNextBytes(1);
			std::cout << "Key range: " << (unsigned int)min << " - " << (unsigned int)max << std::endl;
			break;
		}

		case GEN_VEL_RANGE:
		{
			uint8_t min = *reader.getNextBytes(1);
			uint8_t max = *reader.getNextBytes(1);
			std::cout << "Velocity range: " << (unsigned int)min << " - " << (unsigned int)max << std::endl;
			break;
		}

		case GEN_KEY_NUM:
		{
			uint16_t data = *reader.getNextBytes(2);
			std::cout << "Key number: " << (unsigned int)data << std::endl;
			break;
		}

		case GEN_VELOCITY:
		{
			uint16_t data = *reader.getNextBytes(2);
			std::cout << "Velocity: " << (unsigned int)data << std::endl;
			break;
		}
		default:
			reader.skipNextBytes(2);
			break;
		}
		*/

		//std::cout << "Operation: " << (unsigned int)genOperation << ", range data: " << (unsigned int)((uint8_t)(&data)[0]) << " - " << (unsigned int)((uint8_t)(&data)[1]) << ", signed data: " << (unsigned int)data << ", unsigned data: " << (int) data << std::endl;

		//presetGenAmounts.insert({ genOperation, std::vector<SoundFontPresetGenAmount>()});
		//presetGenAmounts[genOperation].push_back(SoundFontPresetGenAmount{ range, signedAmount, unsignedAmount });
	}
}

void SoundFontFile::parseSampleDataChunk(uint32_t endByte) {
	while (reader.currentByte < endByte) {
		uint32_t chunkType = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));

		switch (chunkType) {
		case 'smpl':
			std::cout << "Sample chunk!" << std::endl;
			parseSampleChunk();
			//skipChunk();
			break;
		case 'sm24':
			std::cout << "24 bit sample chunk!" << std::endl;
			skipChunk();
			break;
		default:
			std::cout << "Unknown chunk!" << std::endl;
			skipChunk();
			break;
		}
	}
}

void SoundFontFile::parsePresetDataChunk(uint32_t endByte) {
	while (reader.currentByte < endByte) {
		uint32_t chunkType = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));

		//std::cout << "Chunk type: " << chunkType << std::endl;
		switch (chunkType) {
		case 'phdr':
			std::cout << "Preset header chunk!" << std::endl;
			parsePresetHeaderChunk();
			//skipChunk();
			break;
		case 'pbag':
			std::cout << "Preset index chunk!" << std::endl;
			parsePresetBagChunk();
			break;
		case 'pmod':
			std::cout << "Preset modulator chunk!" << std::endl;
			skipChunk();
			break;
		case 'pgen':
			std::cout << "Preset generator chunk!" << std::endl;
			parsePresetGenChunk();
			break;
		case 'inst':
			std::cout << "Instrument names and indices chunk!" << std::endl;
			parseInstrumentChunk();
			break;
		case 'ibag':
			std::cout << "Instrument index chunk!" << std::endl;
			parseInstrumentBagChunk();
			break;
		case 'imod':
			std::cout << "Instrument modulator chunk!" << std::endl;
			skipChunk();
			break;
		case 'igen':
			std::cout << "Instrument generator chunk!" << std::endl;
			parseInstrumentGenChunk();
			break;
		case 'shdr':
			std::cout << "Sample header chunk!" << std::endl;
			parseSampleHeaderChunk();
			//skipChunk();
			break;
		default:
			std::cout << "Unknown chunk!" << std::endl;
			skipChunk();
			break;
		}
	}
}

void SoundFontFile::parseList() {
	uint32_t chunkType = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));
	uint32_t endByte = reader.currentByte + *(uint32_t*)reader.getNextBytes(4);
	uint32_t chunkName = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));

	std::cout << "================================================================================" << std::endl;
	switch (chunkType) {
	case 'LIST':
		std::cout << "List chunk!" << std::endl;
			switch (chunkName) {
			case 'INFO':
				std::cout << "Info chunk!" << std::endl;
				parseInfoChunk(endByte);
				break;
			case 'sdta':
				std::cout << "Sample data chunk!" << std::endl;
				parseSampleDataChunk(endByte);
				break;
			case 'pdta':
				std::cout << "Preset data chunk!" << std::endl;
				parsePresetDataChunk(endByte);
				break;
			default:
				std::cout << "Unknown chunk!" << std::endl;
				reader.skipNextBytes(endByte - reader.currentByte);
				break;
			}

		//skipChunk();
		break;
	default:
		std::cout << "Not a list chunk!" << std::endl;
		skipChunk();
		break;
	}
}

void SoundFontFile::organizeSamples() {
	for (unsigned int i = 0; i < 128; i++) {
		samples[i] = new CombinedSample[128];

		for (unsigned int ii = 0; ii < 128; ii++) {
			samples[i][ii] = CombinedSample{ 0, nullptr, 0, 1 };
		}
	}

	//std::cout << "Instrument bags size: " << instrumentBags.size() << std::endl;
	//SoundFontInstrumentBag* globalBag = nullptr;
	uint16_t globalBagIndex = UINT16_MAX;
	for (unsigned int iiii = 0; iiii < instrumentBags.size(); iiii++) { //I should stop naming my loop variables like this
		SoundFontInstrumentBag bag = instrumentBags[iiii];

		//std::cout << "Bag iteration " << iiii << std::endl;

		//Make loop that goes through and finds generators taht are required to put sample in right place in array
		uint16_t instrument;
		float volReleaseSeconds = 1;
		uint16_t sampleId = UINT16_MAX;
		uint16_t rootKey = UINT16_MAX;
		uint16_t keyMin = UINT16_MAX;
		uint16_t keyMax = UINT16_MAX;
		//uint16_t keyNumber = UINT16_MAX;
		//std::cout << "Generator start index: " << (unsigned int) bag.gen << ", next generator start index: " << (unsigned int) instrumentBags[min(iiii + 1, instrumentBags.size() - 1)].gen << ", generators size: " << instrumentGenerators.size() << std::endl;
		for (unsigned int i = bag.gen; i < instrumentBags[min((int) iiii + 1, instrumentBags.size() - 1)].gen; i++) {
			SoundFontGenerator* generator = &instrumentGenerators[i];

			//std::cout << "Generator type: " << generator->type << ", generator data: " << generator->data << std::endl;

			if (generator->type == GEN_INSTRUMENT) {
				//std::cout << "Instrument type" << std::endl;
				instrument = generator->data;
			}
			else if (generator->type == GEN_SAMPLE_ID) {
				//std::cout << "Sample ID type" << std::endl;
				sampleId = generator->data;
			}
			else if (generator->type == GEN_ROOT_KEY) {
				//std::cout << "Root key type" << std::endl;

				rootKey = generator->data;
			}
			else if (generator->type == GEN_KEY_NUM) {
				//std::cout << "Key number type" << std::endl;

				//keyNumber = generator->data;
				keyMin = generator->data;
				keyMax = generator->data;

				//std::cout << "Key number type: " << "min: " << keyMin << ", max: " << keyMax << std::endl;
			}
			else if (generator->type == GEN_KEY_RANGE) {
				uint8_t* range = (uint8_t*)&generator->data;
				keyMin = range[0];
				keyMax = range[1];

				//std::cout << "Key range type: " << "min: " << keyMin << ", max: " << keyMax << std::endl;
			}
			else if (generator->type == GEN_RELEASE_VOL_ENV) {
				volReleaseSeconds = pow(generator->data / 1200.0f, 2.0f);
			}
		}

		if (sampleId == UINT16_MAX) {
			//std::cout << "Sample id is not a thing..." << std::endl;
			//globalBag = &bag;
			globalBagIndex = iiii;
		}

		if (globalBagIndex != UINT16_MAX) {
			//std::cout << "Setting global bag generators" << std::endl;
			//std::cout << "start index: " << instrumentBags[globalBagIndex].gen << ", end index: " << instrumentBags[min(globalBagIndex + 1, instrumentBags.size() - 1)].gen << std::endl;
			for (unsigned int i = instrumentBags[globalBagIndex].gen; i < instrumentBags[min((int) globalBagIndex + 1, instrumentBags.size() - 1)].gen; i ++) {
				SoundFontGenerator* generator = &instrumentGenerators[i];

				if (generator->type == GEN_INSTRUMENT) {
					//std::cout << "Instrument type" << std::endl;
					instrument = generator->data;
				}
				else if (generator->type == GEN_SAMPLE_ID) {
					//std::cout << "Sample ID type" << std::endl;
					sampleId = generator->data;
				}
				else if (generator->type == GEN_ROOT_KEY) {
					//std::cout << "Root key type" << std::endl;

					rootKey = generator->data;
				}
				else if (generator->type == GEN_KEY_NUM) {
					//std::cout << "Key number type" << std::endl;

					//keyNumber = generator->data;
					keyMin = generator->data;
					keyMax = generator->data;

					//std::cout << "Key number type: " << "min: " << keyMin << ", max: " << keyMax << std::endl;
				}
				else if (generator->type == GEN_KEY_RANGE) {
					uint8_t* range = (uint8_t*)&generator->data;
					keyMin = range[0];
					keyMax = range[1];

					//std::cout << "Key range type: " << "min: " << keyMin << ", max: " << keyMax << std::endl;
				}
				else if (generator->type == GEN_RELEASE_VOL_ENV) {
					//std::cout << "Setting global bag volume release envolope" << std::endl;
					volReleaseSeconds = pow(generator->data / 1200.0f, 2.0f);
				}
			}
		}

		//std::cout << "Volume release seconds: " << volReleaseSeconds << std::endl;

		std::vector<SoundFontSampleHeader*> headers; //Contains the sample headers that are to be combined into one sample
		if(sampleId != UINT16_MAX){ //A scope to keep the header variable contained to reduce confusion later
			//std::cout << "Header index: " << headers.size() << std::endl;

			SoundFontSampleHeader* header = &sampleHeaders[sampleId];
			headers.push_back(header);

			if (header->type != SAMPLE_TYPE_MONO) {
				if (header->type == SAMPLE_TYPE_LINKED) {
					std::cout << "Linked sample at index " << sampleId << "! I don't know what to do with this." << std::endl;
					continue;
				}
				else {
					//std::cout << "The sample is of type left or right." << std::endl;
					header = &sampleHeaders[header->link];

					//std::cout << "Header index: " << headers.size() << std::endl;

					headers.push_back(header);
				}
			}
		}

		//std::cout << "Sorting headers..." << std::endl;
		bool sorted = false;
		while (!sorted) {
			sorted = true;
			for (unsigned int ii = 0; ii < headers.size(); ii++) {
				if (headers[ii]->endIndex - headers[ii]->startIndex > headers[min(ii + 1, headers.size() - 1)]->endIndex - headers[min(ii + 1, headers.size() - 1)]->startIndex) {
					uint32_t startIndex = headers[ii]->startIndex;
					uint32_t endIndex = headers[ii]->endIndex;
					headers[ii] = headers[min(ii + 1, headers.size() - 1)];
					headers[min(ii + 1, headers.size() - 1)]->startIndex = startIndex;
					headers[min(ii + 1, headers.size() - 1)]->endIndex = endIndex;
					sorted = false;
				}

				//std::cout << (unsigned int)(headers[ii]->endIndex - headers[ii]->startIndex) << "; ";
			}
			//std::cout << std::endl;
		}

		//std::cout << "Combining audio data..." << std::endl;
		if (rootKey < 128) {
			double rootKeyFrequency = 16.35 * pow(pow(2.0, 1.0 / 12.0), (double)rootKey);
			for (unsigned int iKey = keyMin; iKey <= keyMax; iKey++) {
				//std::cout << "Min key: " << keyMin << "; Max key: " << keyMax << std::endl;
				//std::cout << "Iterating for key " << iKey << std::endl;
				//16.35 * pow(pow(2.0, 1.0 / 12.0), (double)i)
				double keyFrequency = 16.35 * pow(pow(2.0, 1.0 / 12.0), (double)iKey);
				for (unsigned int iii = 0; iii < headers.size(); iii++) {
					SoundFontSampleHeader* header = headers[iii];

					if (samples[0][iKey].buffer == nullptr) {
						samples[0][iKey].bufferSize = (int)((double)(header->endIndex - header->startIndex) * (rootKeyFrequency / keyFrequency)) * 2;
						//std::cout << "Key: " << iKey << "; Header index: " << iii << "; Buffer size: " << samples[0][iKey].bufferSize << "; Buffer: " << samples[0][iKey].buffer << "; Sample rate: " << header->sampleRate << std::endl;
						samples[0][iKey].buffer = new float[samples[0][iKey].bufferSize]; //exception here
						samples[0][iKey].sampleRate = header->sampleRate;
						samples[0][iKey].release = volReleaseSeconds;
						//std::cout << "Buffer size for key " << iKey << ": " << samples[0][iKey].bufferSize << std::endl;
						for (unsigned int ii = 0; ii < samples[0][iKey].bufferSize; ii++) {
							samples[0][iKey].buffer[ii] = 0.0f;
						}
					}

					int16_t* shortData = (int16_t*)rawAudioData;
					if (header->type == SAMPLE_TYPE_MONO) {
						//std::cout << "Putting mono sample in for key " << (unsigned int)iKey << std::endl;
						for (unsigned int ii = header->startIndex; ii < header->endIndex; ii++) {
							samples[0][iKey].buffer[(ii - header->startIndex) * 2] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							samples[0][iKey].buffer[(ii - header->startIndex) * 2 + 1] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //right
						}
					}
					else if (header->type == SAMPLE_TYPE_RIGHT) {
						//std::cout << "Putting right sample in for key " << (unsigned int)iKey << std::endl;
						//std::cout << "max index: " << header->endIndex - header->startIndex << ", multiplier for speedies: " << keyFrequency / rootKeyFrequency << std::endl;
						for (unsigned int ii = 0; (unsigned int)(ii * (keyFrequency / rootKeyFrequency)) < header->endIndex - header->startIndex && ii * 2 + 1 < samples[0][iKey].bufferSize; ii++) {
							//samples[0][generator->data].buffer[(ii - header->startIndex) * 2] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							samples[0][iKey].buffer[ii * 2 + 1] += (shortData[(int)(ii * (keyFrequency / rootKeyFrequency) + header->startIndex)] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //right
						}
					}
					else if (header->type == SAMPLE_TYPE_LEFT) {
						//std::cout << "Putting left sample in for key " << (unsigned int)iKey << std::endl;
						//std::cout << "max index: " << header->endIndex - header->startIndex << ", multiplier for speedies: " << keyFrequency / rootKeyFrequency << std::endl;
						for (unsigned int ii = 0; (unsigned int)(ii * (keyFrequency / rootKeyFrequency)) < header->endIndex - header->startIndex && ii * 2 < samples[0][iKey].bufferSize; ii++) {
							//samples[0][generator->data].buffer[(ii - header->startIndex) * 2] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							samples[0][iKey].buffer[ii * 2] += (shortData[(int)(ii * (keyFrequency / rootKeyFrequency) + header->startIndex)] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							//samples[0][generator->data].buffer[(ii - header->startIndex) * 2 + 1] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //right
						}
					}
				}
			}
		}
	}



		/*int index = 0;
		for (unsigned int i = bag.gen; i < instrumentBags[min(iiii + 1, instrumentBags.size())].gen; i++) {
			SoundFontGenerator* generator = &instrumentGenerators[i];

			//Assuming the instrument is zero!!
			//ROM TYPES ARE CURRENTLY IGNORED!!!!!!
			//INSTRUMENTS ARE CURRENTLY NOT TAKEN INTO ACCOUNT!!##!
			if (generator->type == GEN_ROOT_KEY) {
				//std::cout << "Gathering headers..." << std::endl;
				std::vector<SoundFontSampleHeader*> headers = std::vector<SoundFontSampleHeader*>(); //Contains the sample headers that are to be combined into one sample
				{ //A scope to keep the header variable contained to reduce confusion later
					SoundFontSampleHeader* header = &sampleHeaders[sampleId];
					headers.push_back(header);

					if (header->type != SAMPLE_TYPE_MONO) {
						if (header->type == SAMPLE_TYPE_LINKED) {
							std::cout << "Linked sample at index " << sampleId << "! I don't know what to do with this." << std::endl;
							break;
						}
						else {
							header = &sampleHeaders[header->link];

							headers.push_back(header);
						}
					}
				}

				//std::cout << "Sorting headers..." << std::endl;
				bool sorted = false;
				while (!sorted) {
					sorted = true;
					for (unsigned int ii = 0; ii < headers.size(); ii++) {
						if (headers[ii]->endIndex - headers[ii]->startIndex > headers[min(ii + 1, headers.size() - 1)]->endIndex - headers[min(ii + 1, headers.size() - 1)]->startIndex) {
							uint32_t startIndex = headers[ii]->startIndex;
							uint32_t endIndex = headers[ii]->endIndex;
							headers[ii] = headers[min(ii + 1, headers.size() - 1)];
							headers[min(ii + 1, headers.size() - 1)]->startIndex = startIndex;
							headers[min(ii + 1, headers.size() - 1)]->endIndex = endIndex;
							sorted = false;
						}

						std::cout << (unsigned int)(headers[ii]->endIndex - headers[ii]->startIndex) << "; ";
					}
					std::cout << std::endl;
				}

				//std::cout << "Combining audio data..." << std::endl;
				for (unsigned int iii = 0; iii < headers.size(); iii++) {
					SoundFontSampleHeader* header = headers[iii];

					if (samples[0][generator->data].buffer == nullptr) {
						samples[0][generator->data].bufferSize = (header->endIndex - header->startIndex) * 2;
						samples[0][generator->data].buffer = new float[(header->endIndex - header->startIndex) * 2];
						samples[0][generator->data].sampleRate = header->sampleRate;
						for (unsigned int ii = 0; ii < samples[0][generator->data].bufferSize; ii++) {
							samples[0][generator->data].buffer[ii] = 0.0f;
						}
					}

					int16_t* shortData = (int16_t*)rawAudioData;
					if (header->type == SAMPLE_TYPE_MONO) {
						for (unsigned int ii = header->startIndex; ii < header->endIndex; ii++) {
							samples[0][generator->data].buffer[(ii - header->startIndex) * 2] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							samples[0][generator->data].buffer[(ii - header->startIndex) * 2 + 1] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //right
						}
					}
					else if (header->type == SAMPLE_TYPE_RIGHT) {
						//std::cout << "Putting right sample in for key " << (unsigned int)generator->data << std::endl;
						for (unsigned int ii = header->startIndex; ii < header->endIndex; ii++) {
							//samples[0][generator->data].buffer[(ii - header->startIndex) * 2] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							samples[0][generator->data].buffer[(ii - header->startIndex) * 2 + 1] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //right
						}
					}
					else if (header->type == SAMPLE_TYPE_LEFT) {
						//std::cout << "Putting left sample in for key " << (unsigned int) generator->data << std::endl;
						for (unsigned int ii = header->startIndex; ii < header->endIndex; ii++) {
							//std::cout << ii << ": " << (ii - header->startIndex) * 2 << std::endl;
							samples[0][generator->data].buffer[(ii - header->startIndex) * 2] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //left
							//samples[0][generator->data].buffer[(ii - header->startIndex) * 2 + 1] += (shortData[ii] - INT16_MIN) * (2.0f / (UINT16_MAX)) - 1.0f; //right
						}
					}
				}

				//std::cout << "Sample start index: " << (unsigned int)header->startIndex << ", sample end index: " << (unsigned int)header->endIndex << ", size: " << (unsigned int)header->endIndex - (unsigned int)header->startIndex << std::endl;
				//std::cout << "Key: " << generator->data << std::endl;
				(start internal comment)
				while (header->type != SAMPLE_TYPE_MONO && header->type != SAMPLE_TYPE_ROM_MONO) {
					samples[0][generator->data].bufferSize = (header->endIndex - header->startIndex);
					samples[0][generator->data].buffer = new float[(header->endIndex - header->startIndex)];

					for (unsigned int ii = header->startIndex * 2; ii < header->endIndex * 2; ii++) {
						samples[0][generator->data].buffer[ii - header->startIndex * 2] += rawAudioData[ii];
					}

					index++;

					header = &sampleHeaders[index];
				}
				(end internal comment)
			}
		}
	}
	*/

	/*
	for (unsigned int i = 0; i < sampleHeaders.size(); i++) {
		SoundFontSampleHeader* header = &sampleHeaders[i];

		//Assuming the instrument is zero!!
		samples[0][header->key] = SoundFontSample{ (header->endIndex - header->startIndex) * 2, new uint8_t[(header->endIndex - header->startIndex) * 2] };

		for (unsigned int ii = header->startIndex * 2; ii < header->endIndex * 2; ii++) {
			samples[0][header->key].sample[ii - header->startIndex * 2] = rawAudioData[ii];
		}
	}
	*/
}

void SoundFontFile::parseSfbkChunk() {
	uint32_t chunkType = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));
	uint32_t endByte = reader.currentByte + *(uint32_t*)reader.getNextBytes(4);
	uint32_t chunkName = _byteswap_ulong(*(unsigned long*)reader.getNextBytes(4));

	switch (chunkType) {
	case 'RIFF':
		std::cout << "This is a RIFF file!" << std::endl;
		switch (chunkName) {
		case 'sfbk':
			std::cout << "This is a soundfont file!" << std::endl;

			while (reader.currentByte < endByte) {
				parseList();
			}

			break;
		default:
			std::cout << "This is not a soundfont file!" << std::endl;
		}

			break;
	default:
		std::cout << "This is not a RIFF file!" << std::endl;
		break;
	}
}

void SoundFontFile::load(std::string filePath) {
	reader = BufferedReader(filePath, 1048576 * 256); //buffer is 256 bytes

	parseSfbkChunk();

	reader.close();

	std::cout << "Organizing samples..." << std::endl;
	organizeSamples();
	std::cout << "Done." << std::endl;

	/*
	for (unsigned int i = 0; i < 128; i++) {
		std::cout << "Sample key " << i << " with size " << samples[0][i].bufferSize << ": " << std::endl;
		for (unsigned int ii = 0; ii < min(100, samples[0][i].bufferSize); ii++) {
			std::cout << (unsigned int)samples[0][i].sample[ii] << "; ";
		}
		std::cout << std::endl;
	}
	*/

	delete[] rawAudioData;
}