#include <chrono>
#include "Timer.h"

void Timer::start() { startTime = std::chrono::high_resolution_clock::now(); ticking = true; /*this->startingTime = std::chrono::microseconds(0);*/ }

/*
void Timer::start(std::chrono::microseconds startingTime) {
	this->startingTime = startingTime;
	startTime = std::chrono::system_clock::now();
}
*/

void Timer::stop() { endTime = std::chrono::high_resolution_clock::now(); ticking = false; }

uint64_t Timer::getElapsedMicroseconds() {
	return ticking * std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
}

uint64_t Timer::getCurrentMicroseconds() { return ticking * std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime).count(); } // + this->startingTime