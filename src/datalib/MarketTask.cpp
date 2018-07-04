#include "datalib/MarketTask.h"
#include "common/AppLog.h"

//namespace itstation{

TimedStateTaskManager::TimedStateTaskManager() : StateTaskManager(), interval_(60 * 1000), start_sec_(0)
	, day_open_time_(DAY_OPEN_TIME), day_close_time_(DAY_CLOSE_TIME)
	, night_open_time_(NIGHT_OPEN_TIME), night_close_time_(NIGHT_CLOSE_TIME)
{
	try {
		InsertTask(new NonTradingTask(this));
		InsertTask(new DayOpenTask(this));
		InsertTask(new DayCloseTask(this));
		InsertTask(new NightOpenTask(this));
		InsertTask(new NightCloseTask(this));
	}
	catch (ExceptionErr& err)
	{
		cout << err.what() << endl;
	}
	
	SetCurTask(NON_TRADING_ID);
}

TimedStateTaskManager::~TimedStateTaskManager()
{
	//timer_api_->Stop();
	delete timer_api_;
}

void TimedStateTaskManager::SetInterval(int interval)
{
	if(interval <= 0) { return; }
	interval_ = interval;
	timer_api_ = new TimerApi(interval_, this);
}

void TimedStateTaskManager::SetStartSec(int sec)
{
	start_sec_ = sec;
}

void TimedStateTaskManager::StartTimer()
{
	timer_api_->Start(start_sec_);
	
}

void TimedStateTaskManager::OnTimer()
{
	try
	{
		HandleNextTask();
	}
	catch (ExceptionErr* e)
	{
		cout << e->what() << endl;
		APP_LOG(LOG_LEVEL_ERROR) << e->what();
		timer_api_->Stop();
	}
	catch (...) {
		assert(false);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////

bool NonTradingTask::IsNextHoliday(Date date)
{
	++date;
	while (date.WeekDay() > 5)
	{
		if (date.IsHoliday()) return true;
		++date;
	}
	return date.IsHoliday();
}

int NonTradingTask::ToNextState()
{
	DateTime now(NULL);
	if (now.time > manager_->day_open_time_ && now.time < manager_->day_close_time_ && now.IsTradingDay())
	{
		return DAY_OPEN_ID;
	}

	if ((now.time > manager_->night_open_time_ && now.IsTradingDay() && !IsNextHoliday(now.date)))
	{
		return NIGHT_OPEN_ID;
	}

	//对于凌晨2.30以前，需要退回到前一日再验证
	if (now.time < manager_->night_close_time_)
	{
		Date date = now.date;
		--date;
		if (date.IsTradingDay() && !IsNextHoliday(date))
		{
			return NIGHT_OPEN_ID;
		}
	}

	return -1;
}

int DayOpenTask::ToNextState()
{
	DateTime now(NULL);
	return now.time > manager_->day_close_time_ ? DAY_CLOSE_ID : -1;
}

int DayCloseTask::ToNextState()
{
	return NON_TRADING_ID;
}

int NightOpenTask::ToNextState()
{
	DateTime now(NULL);
	return now.time > manager_->night_close_time_ && now.time < manager_->day_open_time_ ? NIGHT_CLOSE_ID : -1;
}

int NightCloseTask::ToNextState()
{
	return NON_TRADING_ID;
}

