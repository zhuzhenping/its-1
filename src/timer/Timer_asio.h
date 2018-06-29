#pragma once
#include "timer/Timer.h"
#include <boost/timer.hpp>
#include <boost/asio.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <thread>
#include <atomic>
#include <functional>

using namespace boost::gregorian;
using namespace boost::posix_time;

class Timer_asio : public Timer	
{
public:
	Timer_asio(char begin_time[31], int interval, TimerSpi *spi);
	virtual ~Timer_asio();

	// Æô¶¯.
	virtual bool StartUp(std::string &err);
	// Í£Ö¹.
	virtual void TearDown();

	bool isActive() const
	{
		return is_active_;
	}

private:
	boost::asio::io_service io_service_;
	boost::asio::deadline_timer timer_;
	std::thread thread_;
	bool is_active_;

	ptime begin_time_;
	milliseconds interval_;
	time_iterator titr_;
	std::function<void()> func_;

	//void OnTimer(const boost::system::error_code&);
};

