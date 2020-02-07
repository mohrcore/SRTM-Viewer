#include "clock.h"

#ifdef POSIX_PRECISE_CLOCK
#define BILLION 1000000000

Clock::Clock() {
    this->reset();
}

void Clock::reset() {
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &this->_timespec_zero);
}

double Clock::getElapsedTime() {
    timespec now;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &now);
    __syscall_slong_t nsdiff = now.tv_nsec - this->_timespec_zero.tv_nsec;
    __time_t sdiff = now.tv_sec - this->_timespec_zero.tv_sec;
    if (nsdiff < 0) {
        sdiff -= 1;
        nsdiff = BILLION + nsdiff;
    }
    return double(sdiff) + double(nsdiff / BILLION);
}
#else
Clock::Clock() {
    this->reset();
}

void Clock::reset() {
    this->_timepoint_zero = std::chrono::high_resolution_clock::now();
}

double Clock::getElapsedTime() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::chrono::high_resolution_clock::period> diff = now - this->_timepoint_zero;
    return double(diff.count() * std::chrono::high_resolution_clock::period::num) / double(std::chrono::high_resolution_clock::period::den);
}
#endif