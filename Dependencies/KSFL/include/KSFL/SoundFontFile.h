#pragma once
#include <string>
#include "BufferedReader.h"
#include <vector>

struct CombinedSample {
	unsigned int bufferSize;
	float* buffer;
	uint32_t sampleRate;
	float release;
};

struct SoundFontSampleHeader { //Note: pitch correction not implemented here
	uint32_t startIndex;
	uint32_t endIndex;
	uint32_t startLoop;
	uint32_t endLoop;
	uint32_t sampleRate;
	uint8_t key;
	uint16_t link;
	uint16_t type;
};

struct SoundFontPresetHeader {
	uint16_t preset;
	uint16_t bank;
	uint16_t presetBag;
};

struct SoundFontPresetBag {
	uint16_t gen;
	uint16_t mod;
};

struct SoundFontGenerator {
	uint16_t type;
	uint16_t data;
};

struct SoundFontInstrumentBag {
	uint16_t gen;
	uint16_t mod;
};

class SoundFontFile {
public:
	static const uint16_t SAMPLE_TYPE_MONO = 1;
	static const uint16_t SAMPLE_TYPE_RIGHT = 2;
	static const uint16_t SAMPLE_TYPE_LEFT = 4;
	static const uint16_t SAMPLE_TYPE_LINKED = 8;
	static const uint16_t SAMPLE_TYPE_ROM_MONO = 32769;
	static const uint16_t SAMPLE_TYPE_ROM_RIGHT = 32770;
	static const uint16_t SAMPLE_TYPE_ROM_LEFT = 32772;
	static const uint16_t SAMPLE_TYPE_ROM_LINKED = 32776;

	static const uint16_t GEN_RELEASE_VOL_ENV = 38;
	static const uint16_t GEN_INSTRUMENT = 41;
	static const uint16_t GEN_KEY_RANGE = 43;
	static const uint16_t GEN_VEL_RANGE = 44;
	static const uint16_t GEN_KEY_NUM = 46;
	static const uint16_t GEN_VELOCITY = 47;
	static const uint16_t GEN_SAMPLE_ID = 53;
	static const uint16_t GEN_ROOT_KEY = 58;

	CombinedSample** samples = new CombinedSample*[128];

	uint8_t* rawAudioData;

	std::vector<SoundFontSampleHeader> sampleHeaders;

	std::vector<SoundFontPresetHeader> presetHeaders;

	std::vector<SoundFontPresetBag> presetBags;

	std::vector<uint16_t> instrumentIndexes;
	
	std::vector<SoundFontInstrumentBag> instrumentBags;

	std::vector<SoundFontGenerator> presetGenerators;

	std::vector<SoundFontGenerator> instrumentGenerators;

	//std::map<uint16_t, std::vector<SoundFontPresetGenAmount>> presetGenAmounts;

	SoundFontFile();

	~SoundFontFile();

	SoundFontFile(std::string filePath);

	void load(std::string filePath);

private:
	BufferedReader reader;

	void organizeSamples();

	void skipChunk();

	void parseInstrumentGenChunk();

	void parseInstrumentChunk();

	void parseInstrumentBagChunk();

	void parsePresetGenChunk();

	void parsePresetBagChunk();

	void parsePresetHeaderChunk();

	void parseSampleChunk();

	void parseSampleHeaderChunk();

	void parseList();

	void parseInfoChunk(uint32_t endByte);

	void parseSampleDataChunk(uint32_t endByte);

	void parsePresetDataChunk(uint32_t endByte);

	void parseSfbkChunk();
};