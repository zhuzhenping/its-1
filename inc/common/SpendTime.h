// Copyright (C) by Ashton Mason. See LICENSE.txt for licensing information.
#ifndef THERON_BENCHMARKS_COMMON_TIMER_H
#define THERON_BENCHMARKS_COMMON_TIMER_H

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif


// Simple timer class.
class Timer
{
public:

	Timer() : mSupported(false)
	{
#ifdef WIN32
		// Read the counter frequency (in Hz) and an initial counter value.
		if (QueryPerformanceFrequency(&mTicksPerSecond) && QueryPerformanceCounter(&mCounterStartValue))
		{
			mSupported = true;
		}
#else
		mSupported = true;
#endif
	}

	void Start()
	{
#ifdef WIN32
		QueryPerformanceCounter(&mCounterStartValue);
#else
		gettimeofday(&t1, NULL);
#endif
	}

	void Stop()
	{
#ifdef WIN32
		QueryPerformanceCounter(&mCounterEndValue);
#else
		gettimeofday(&t2, NULL);
#endif
	}

	bool Supported() const
	{
		return mSupported;
	}

	double Seconds() const
	{
#ifdef WIN32
		const double elapsedTicks(static_cast<double>(mCounterEndValue.QuadPart - mCounterStartValue.QuadPart));
		const double ticksPerSecond(static_cast<double>(mTicksPerSecond.QuadPart));
		return (elapsedTicks / ticksPerSecond);
#else
		return (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) * 1e-6f;
#endif
	}

private:

	Timer(const Timer &other);
	Timer &operator=(const Timer &other);

	bool mSupported;

#ifdef WIN32
	LARGE_INTEGER mTicksPerSecond;
	LARGE_INTEGER mCounterStartValue;
	LARGE_INTEGER mCounterEndValue;
#else
	timeval t1, t2;
#endif

};


#endif // THERON_BENCHMARKS_COMMON_TIMER_H

