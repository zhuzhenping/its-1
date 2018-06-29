#ifndef KIIIK_TIMER_H_  
#define KIIIK_TIMER_H_  

#include "boost/asio.hpp"
#include "boost/bind.hpp" 
#include "boost/date_time/posix_time/posix_time.hpp"

#include "Common/Thread.h"
#include "NetworkAsio/TcpSession.h"

using namespace itstation;

namespace network_asio {

class TimerSpi
{
public:
	virtual void OnTimer() = 0;
};


class NETWORK_ASIO_API TimerApi : public common::Thread
{
public:
	TimerApi(int ms/*多久触发一次*/, TimerSpi *spi);
	~TimerApi();
	void Start(int ms/*隔多久启动*/ = 0);
	void Stop();

private:
	virtual void OnRun(); 
	void OnTimer(const boost::system::error_code&);

	boost::asio::io_service io_;
	int ms_;
	boost::asio::deadline_timer t_;
	TimerSpi *spi_;

	bool is_running_;
};

}

#endif