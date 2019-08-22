#include "highaccuracytimer.h"

HighAccuracyTimer::HighAccuracyTimer()
{
#ifdef WIN32
    timeStart.QuadPart = 0;
    timeEnd.QuadPart = 0;
    QueryPerformanceFrequency(&CPUFrequency);
#elif __linux__
    timeStart.tv_sec = 0;
    timeStart.tv_usec = 0;
    timeEnd.tv_sec = 0;
    timeEnd.tv_usec = 0;
#endif
}

HighAccuracyTimer::~HighAccuracyTimer()
{

}

void HighAccuracyTimer::start()
{
#ifdef WIN32
    QueryPerformanceCounter(&timeStart);
#elif __linux__
    gettimeofday(&timeStart, nullptr);
#endif
}

double HighAccuracyTimer::elapsed()
{
#ifdef WIN32
    QueryPerformanceCounter(&timeEnd);
    interval = (double(timeEnd.QuadPart) - double(timeStart.QuadPart)) * 1000 / double(CPUFrequency.QuadPart);
#elif __linux__
    gettimeofday(&timeEnd, nullptr);
    interval = (timeEnd.tv_sec - timeStart.tv_sec) * 1000 + (timeEnd.tv_usec - timeStart.tv_usec) / 1000;
#endif
    return interval;
}
