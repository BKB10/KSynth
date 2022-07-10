#include "BufferedReader.h"
#include <iostream>

/*

Buffered Reader

*/
BufferedReader::BufferedReader(std::string filePath, unsigned int _bufferSize) {
	bufferSize = _bufferSize;
	buffer = new char[bufferSize];

	file = std::ifstream(filePath, std::ios::in | std::ios::binary);

	//get length of file
	file.seekg(0, std::ios::end);
	fileLength = file.tellg();
	file.seekg(0, std::ios::beg);
}

uint8_t* BufferedReader::BufferedReader::getNextBytes(uint64_t numBytes) {
	if (currentByte >= bufferPosition * bufferSize) {
		fillBufferWithPosition(bufferPosition++);
	}

	char* bytes = new char[numBytes];
	for (int i = 0; i < numBytes; i++) {
		if (currentByte >= bufferPosition * bufferSize) {
			fillBufferWithPosition(bufferPosition++);
		}

		bytes[i] = buffer[currentByte - bufferSize * (bufferPosition - 1)];

		currentByte++;
	}

	return (uint8_t*)bytes;
}

void BufferedReader::BufferedReader::skipNextBytes(uint64_t numBytes) {
	currentByte += numBytes;

	while (currentByte >= bufferPosition * bufferSize) {
		fillBufferWithPosition(bufferPosition++);
	}
}

void BufferedReader::BufferedReader::pushBack(uint64_t numBytes) {
	currentByte -= numBytes;

	while (currentByte < (bufferPosition - 1) * bufferSize) {
		fillBufferWithPosition(bufferPosition--);
	}
}

void BufferedReader::BufferedReader::fillBufferWithPosition(uint64_t position) {
	std::cout << "Filling buffer" << std::endl;

	delete[] buffer;

	buffer = new char[bufferSize];
	file.seekg(bufferSize * position, file.beg);
	file.read(buffer, bufferSize);
}

void BufferedReader::BufferedReader::close() {
	file.close();

	delete[] buffer;
}