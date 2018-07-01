#include "network/Timer.h"

//namespace network_asio {

TimerApi::TimerApi(int ms, TimerSpi *spi) 
	: spi_(spi)
	, ms_(ms)	
	, t_(io_, boost::posix_time::milliseconds(ms_))
	, is_running_(false)
{
	Thread::Start();
}

TimerApi::~TimerApi(){
	io_.stop();
	Thread::Stop();	
}

void TimerApi::Start(int ms){
	if (!is_running_) {
		is_running_ = true;
		t_.expires_at(t_.expires_at() + boost::posix_time::milliseconds(ms<=0?ms_:ms));  
		t_.async_wait(boost::bind(&TimerApi::OnTimer, this,
			boost::asio::placeholders::error));
	}	
}

void TimerApi::Stop(){
	is_running_ = false;
	/*Thread::Stop();
	io_.stop();*/
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

