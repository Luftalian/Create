#pragma once

#ifndef TIMER_H
#define TIMER_H
#include <Arduino.h>

class Timer
{
public:
    unsigned long Gettime_record();
    // {
    //     time = micros();
    //     time -= start_time;
    //     return time;
    // }
    unsigned long start_time;
    unsigned long time;
    bool start_flag = true;
};

unsigned long Timer::Gettime_record()
{
    time = micros();
    time -= start_time;
    return time;
}

#endif