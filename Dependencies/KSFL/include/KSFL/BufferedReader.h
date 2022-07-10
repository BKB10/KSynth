#pragma once
#include <fstream>

class BufferedReader {
public:
	unsigned int bufferSize; //In bytes
	uint64_t bufferPosition = 0;
	uint64_t currentByte = 0;
	uint64_t fileLength;

	BufferedReader() {}

	BufferedReader(std::string filePath, unsigned int _bufferSize);

	uint8_t* getNextBytes(uint64_t numBytes);

	void skipNextBytes(uint64_t numBytes);

	void pushBack(uint64_t numBytes);

	void close();

private:
	std::ifstream file;
	char* buffer = nullptr;

	void fillBufferWithPosition(uint64_t position);
};