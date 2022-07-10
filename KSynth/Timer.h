#pragma once

#include <chrono>

class Timer
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    //std::chrono::microseconds startingTime;

public:
    bool ticking = false;

    Timer() {}

    void start();

    //void start(std::chrono::microseconds startingTime);

    void stop();

    uint64_t getElapsedMicroseconds();

    uint64_t getCurrentMicroseconds();
};