#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
private:
    std::chrono::system_clock::time_point last;
    std::chrono::system_clock::time_point now;

public:
    Timer();

    double tick();
    void reset();
};

#endif // TIMER_H
