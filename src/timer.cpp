#include "timer.hpp"

void Timer::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    endTime_ = std::chrono::high_resolution_clock::now();
}

double Timer::elapsedMilliseconds() const {
    return std::chrono::duration<double, std::milli>(endTime_ - startTime_).count();
}