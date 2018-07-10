#include "strategy/AutoRun.h"
#include "common/DateTime.h"
#include <common/AppLog.h>
#include <iostream>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QStringList>



AutoRun::AutoRun() : TimedStateTaskManager(), strategy_(NULL)
{
	day_close_time_ = Time(15, 20, 0);
	night_open_time_ = Time(20, 40, 0);
	SetInterval(10000);
	StartTimer();
	
	strategy_ = new Strategy;
}

AutoRun::~AutoRun(void)
{
	if (strategy_) {
		delete strategy_;
		strategy_ = NULL;
	}
}

bool AutoRun::DoDayOpen(){
	APP_LOG(LOG_LEVEL_INFO) << "DoDayOpen";
	string err;
	if (!strategy_->Init(err))
	{
		APP_LOG(LOG_LEVEL_ERROR)<<"DoDayOpen fail:"<<err;
		return false;
	}
	return true;
}

bool AutoRun::DoDayClose(){
	if (NULL != strategy_)
	{
		strategy_->Denit();
		APP_LOG(LOG_LEVEL_INFO) << "DoDayClose";
	}
	return true;
}

bool AutoRun::DoNightOpen(){
	APP_LOG(LOG_LEVEL_INFO)<<"DoNightOpen";
	string err;
	if (!strategy_->Init(err))
	{
		APP_LOG(LOG_LEVEL_ERROR)<<"DoNightOpen fail:"<<err;
		return false;
	}
	return true;
}

bool AutoRun::DoNightClose(){
	if (NULL != strategy_)
	{
		strategy_->Denit();
		APP_LOG(LOG_LEVEL_INFO)<<"DoNightClose";
	}
	return true;
}

