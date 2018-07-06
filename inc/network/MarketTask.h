#ifndef ITS_STRATEGY_MARKET_TASK_H_
#define ITS_STRATEGY_MARKET_TASK_H_

#include "network/StateTask.h"
#include "network/Timer.h"
#include "common/DateTime.h"

//using namespace network_asio;

//namespace itstation{

#define DAY_OPEN_TIME Time(8, 45, 0)
#define DAY_CLOSE_TIME Time(15, 35, 0)
#define NIGHT_OPEN_TIME Time(19, 45, 0)
#define NIGHT_CLOSE_TIME Time(2, 35, 0)

#define NON_TRADING_ID	0
#define DAY_OPEN_ID		1
#define DAY_CLOSE_ID	2
#define NIGHT_OPEN_ID	3
#define NIGHT_CLOSE_ID	4

class NETWORK_API TimedStateTaskManager : public StateTaskManager, public TimerSpi
{
public:
	friend class NonTradingTask; 
	friend class DayOpenTask; 
	friend class DayCloseTask;
	friend class NightOpenTask; 
	friend class NightCloseTask;

	TimedStateTaskManager();
	virtual ~TimedStateTaskManager();

	void SetInterval(int interval);//毫秒.
	void SetStartSec(int sec);//毫秒.
	void StartTimer();

	virtual bool DoDayOpen() = 0;
	virtual bool DoDayClose() = 0;
	virtual bool DoNightOpen() = 0;
	virtual bool DoNightClose() = 0;

private:
	virtual void OnTimer();

protected:
	int interval_;
	int start_sec_;

	TimerApi *timer_api_;

	Time day_open_time_;
	Time day_close_time_;
	Time night_open_time_;
	Time night_close_time_;
};

class NonTradingTask : public StateTask
{
public:
	NonTradingTask(TimedStateTaskManager* manager) : manager_(manager), StateTask(NON_TRADING_ID) {}
	virtual bool Handle(StateTaskManager* manager) { return true; }
	virtual int ToNextState();

	bool IsNextHoliday(Date date);

private:
	TimedStateTaskManager* manager_;
};

class DayOpenTask : public StateTask
{
public:
	DayOpenTask(TimedStateTaskManager* manager) : StateTask(DAY_OPEN_ID), manager_(manager) {}
	virtual bool Handle(StateTaskManager* manager) { return manager_->DoDayOpen(); }
	virtual int ToNextState();

private:
	TimedStateTaskManager* manager_;
};

class DayCloseTask : public StateTask
{
public:
	DayCloseTask(TimedStateTaskManager* manager) : StateTask(DAY_CLOSE_ID), manager_(manager) {}
	virtual bool Handle(StateTaskManager* manager) { return manager_->DoDayClose(); }
	virtual int ToNextState();

private:
	TimedStateTaskManager* manager_;
};

class NightOpenTask : public StateTask
{
public:
	NightOpenTask(TimedStateTaskManager* manager) : StateTask(NIGHT_OPEN_ID), manager_(manager) {}
	virtual bool Handle(StateTaskManager* manager) { return manager_->DoNightOpen(); }
	virtual int ToNextState();

private:
	TimedStateTaskManager* manager_;
};

class NightCloseTask : public StateTask
{
public:
	NightCloseTask(TimedStateTaskManager* manager) : StateTask(NIGHT_CLOSE_ID), manager_(manager) {}
	virtual bool Handle(StateTaskManager* manager) { return manager_->DoNightClose(); }
	virtual int ToNextState();

private:
	TimedStateTaskManager* manager_;
};



#endif