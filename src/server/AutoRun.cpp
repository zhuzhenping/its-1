#include "AutoRun.h"
#include "common/DateTime.h"
#include <common/AppLog.h>
#include <iostream>
#include <QtCore/QDateTime>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QStringList>



AutoRun::AutoRun() : TimedStateTaskManager(), ctp_client_(NULL)
{
	day_close_time_ = Time(15, 20, 0);
	night_open_time_ = Time(20, 40, 0);
	ctp_client_ = new CtpClient();
	instrument_table_ = new MySecurityInfoSpi(PRODUCT_FUTURE);
}

AutoRun::~AutoRun(void)
{
	delete ctp_client_;
	delete instrument_table_;
}

void AutoRun::Init()
{
	/*std::string err;
	if (!ctp_client_->InitTcp(err))
	{
		APP_LOG(LOG_LEVEL_ERROR)  << "tcp listen error: " << err ;
		return;
	}*/

	SetInterval(10000);
	StartTimer();
}

bool AutoRun::DoDayOpen(){
	APP_LOG(LOG_LEVEL_INFO) << "DoDayOpen";
	string err;
	if (!ctp_client_->Init(true, err))
	{
		APP_LOG(LOG_LEVEL_ERROR)<<"DoDayOpen fail:"<<err;
		return false;
	}
	return true;
}

bool AutoRun::DoDayClose(){
	if (NULL != ctp_client_)
	{
		ctp_client_->Denit();
		APP_LOG(LOG_LEVEL_INFO) << "DoDayClose";
	}
	return true;
}

bool AutoRun::DoNightOpen(){
	APP_LOG(LOG_LEVEL_INFO)<<"DoNightOpen";
	string err;
	if (instrument_table_->Init()) {
		instrument_table_->Denit();
		APP_LOG(LOG_LEVEL_ERROR)<<"get instrument_table_  fail";
	}

	if (!ctp_client_->Init(false, err))
	{
		APP_LOG(LOG_LEVEL_ERROR)<<"DoNightOpen fail:"<<err;
		return false;
	}
	return true;
}

bool AutoRun::DoNightClose(){
	if (NULL != ctp_client_)
	{
		ctp_client_->Denit();
		APP_LOG(LOG_LEVEL_INFO)<<"DoNightClose";
	}
	return true;
}

