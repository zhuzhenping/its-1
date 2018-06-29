#include "timer/Timer.h"
#include <string.h>
#include "Timer_asio.h"


Timer *Timer::CreateTimer(char begin_time[31], int interval, TimerSpi *spi)
{
	return new Timer_asio(begin_time, interval, spi);
}


Timer::~Timer()
{
}

