#pragma once

#ifdef POSIX_PRECISE_CLOCK
#include <time.h>
class Clock {
private:
    timespec _timespec_zero;
public:
    Clock();
    void reset();
    double getElapsedTime();
};
#else
#include <chrono>
class Clock {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _timepoint_zero;
public:
    Clock();
    void reset();
    double getElapsedTime();
};
#endif