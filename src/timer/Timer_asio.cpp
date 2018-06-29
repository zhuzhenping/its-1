#include "Timer_asio.h"
#include <string.h>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/timer.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <boost/date_time.hpp>
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#include "boost/thread.hpp"

using namespace boost;
using namespace boost::this_thread;
using namespace boost::asio;
using namespace boost::gregorian;
using namespace boost::posix_time;

Timer_asio::Timer_asio(char begin_time[31], int interval, TimerSpi *spi)
: timer_(io_service_)
//, begin_time_(from_simple_string(begin_time))
, interval_(interval)
, titr_(begin_time_, interval_)
{
	spi_ = spi;

	// local_time - 8h  -> utc 
	function<ptime(ptime&)> local_to_utc = [&](ptime &local_tm){
		typedef boost::date_time::local_adjustor< boost::posix_time::ptime, +8, boost::posix_time::no_dst> sct_shz;
		return sct_shz::local_to_utc(local_tm);
	};
	ptime local_tm = time_from_string(begin_time);
	begin_time_ = local_to_utc(local_tm);

	titr_ = time_iterator(begin_time_, interval_);
	
	thread_ = std::thread([this] { 
		boost::asio::io_service::work work(io_service_);
		io_service_.run(); 
	});
}


Timer_asio::~Timer_asio()
{
}

// Æô¶¯.
bool Timer_asio::StartUp(std::string &err){
	if (io_service_.stopped())return false;

	is_active_ = true;

	ptime now = second_clock::universal_time();
	
	timer_.expires_at(begin_time_>now?begin_time_:now);
	func_ = [this]{
		timer_.async_wait([this](const boost::system::error_code &ec){
			if (ec) return;

			if (spi_)spi_->OnTimer();

			++titr_;
			boost::system::error_code error_code;
			timer_.expires_at(*titr_, error_code);
			func_();
		});
	};

	func_();
	return true;
}
// Í£Ö¹.
void Timer_asio::TearDown() {
	io_service_.stop();
	if (thread_.joinable())
	{
		thread_.join();
	}
	is_active_ = false;
}