#include "network/Timer.h"
#include <boost/date_time.hpp>
#include <boost/functional.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::gregorian;
using namespace boost::posix_time;

TimerApi::TimerApi(int ms, TimerSpi *spi) 
	: spi_(spi)
	, ms_(ms)	
	, t_(io_, boost::posix_time::milliseconds(0))
	, is_running_(false)
{
}

TimerApi::~TimerApi(){
	io_.stop();
	Thread::Stop();	
	Thread::Join();
}

void TimerApi::Start(int ms){
	if (!is_running_) {
		is_running_ = true;
		t_.expires_at(t_.expires_at() + boost::posix_time::milliseconds(ms<=0?ms_:ms));  
		t_.async_wait(boost::bind(&TimerApi::OnTimer, this,
			boost::asio::placeholders::error));
	}	
	Thread::Start();
}


void TimerApi::Stop(){
	is_running_ = false;
	Thread::Stop();
	Thread::Join();
	//io_.stop();
}

void TimerApi::OnTimer(const boost::system::error_code &_Err){
	if (is_running_) {

		t_.expires_at(t_.expires_at() + boost::posix_time::milliseconds(ms_));  
		t_.async_wait(boost::bind(&TimerApi::OnTimer, this,
			boost::asio::placeholders::error));

		if (spi_) spi_->OnTimer();
	}
}

void TimerApi::OnRun(){
	while (IsRuning())
	{
		boost::asio::io_service::work work(io_);
		io_.run();

	}
}
