#include "timer.h"


Timer::Timer()
{
    now = std::chrono::system_clock::now();
}

double Timer::tick()
{
    last = now;
    now = std::chrono::system_clock::now();
    return (double) std::chrono::duration_cast<std::chrono::microseconds>(now-last).count() / 1000000;
}

void Timer::reset()
{
    now = std::chrono::system_clock::now();
}
