#ifndef KIIIK_TIMER_H_  
#define KIIIK_TIMER_H_  

/*
class MyTimerSpi : public TimerSpi {
public:
	virtual void OnTimer(){
		APP_LOG(LOG_LEVEL_INFO)<<"1";
		Thread::Sleep(3000);
	}
	MyTimerSpi(){
		api = new TimerApi(10000, this);
	}
	TimerApi *api;
};
// 从下一分钟开始启动定时器.
// 即使OnTimer()中休眠，也会严格按照每10秒触发一次.
void main() {
	MyTimerSpi spi;
	DateTime now(NULL);
	Time next = now.time;
	next.sec = 0;
	next.milsec = 0;
	next.AddMin(1);
	spi.api->Start((next-now.time)*1000);
}
*/
#include "boost/asio.hpp"
#include "boost/bind.hpp" 
#include "boost/date_time/posix_time/posix_time.hpp"
#include "common/DateTime.h"
#include "common/Thread.h"

class TimerSpi
{
public:
	virtual void OnTimer() = 0;
};

// put TimerApi() and Start() together
class NETWORK_API TimerApi : public Thread
{
public:
	TimerApi(int ms, TimerSpi *spi);// interval
	~TimerApi();
	void Start(int ms = 0); // after..then start
	//void Start(const DateTime &begin_time); // from begin_time,then start
	//void Start(const string &begin_time); // "2018-07-17 08:08:08"
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



#endif