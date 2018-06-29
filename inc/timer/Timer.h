/*!
* \brief       定时器.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
class MySpi : public TimerSpi {
public:
	virtual void OnTimer(){
		//cout << boost::posix_time::second_clock::local_time()<<"\t";
	}

	MySpi() {
		// 每隔1分钟触发一次.
		timer_ =  Timer::CreateTimer("2017-6-27 10:09:10", 5000, this);
		string err;
		timer_->StartUp(err);
	}

	Timer *timer_;
};
*
*/


#pragma once

#include <string>

#ifdef TIMER_EXPORT
#define TIMER_API  __declspec(dllexport)
#else 
#define TIMER_API  __declspec(dllimport)
#endif

class TimerSpi {
public:
	virtual void OnTimer() = 0;
};

class TIMER_API Timer
{
public:
	// begin_time时间格式"2017-06-26 11:11:00". 
	// interval_time间隔时间 单位为毫秒.
	static Timer *CreateTimer(char begin_time[31], int interval, TimerSpi *spi);
	virtual ~Timer();

	// 启动.
	virtual bool StartUp(std::string &err) = 0;
	// 停止.
	virtual void TearDown() = 0;

	virtual bool isActive() const = 0;

protected:
	TimerSpi *spi_;
	

};

