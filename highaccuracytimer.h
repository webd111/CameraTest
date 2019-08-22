#ifndef HIGHACCURACYTIMER_H
#define HIGHACCURACYTIMER_H

#ifdef WIN32
#include <Windows.h>
#elif __linux__
#include<sys/time.h>
#endif

class HighAccuracyTimer
{
#ifdef WIN32
    LARGE_INTEGER timeStart;
    LARGE_INTEGER timeEnd;
    LARGE_INTEGER CPUFrequency;
#elif __linux__
    struct timeval timeStart;
    struct timeval timeEnd;
#endif
    double interval = 0;
    
public:
    HighAccuracyTimer();
    ~HighAccuracyTimer();
    
    void start();
    double elapsed();
};

#endif
