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
}

AutoRun::~AutoRun(void)
{
	delete ctp_client_;
}

void AutoRun::StartUp()
{
	std::string err;
	if (!ctp_client_->InitTcp(err))
	{
		cout << "tcp listen error: " << err << endl;
		return;
	}

	SetInterval(10000);
	StartTimer();
}

bool AutoRun::DoDayOpen(){
	string err;
	if (!ctp_client_->StartUp(true, err))
	{
		cout << "日盘初始化失败: " << err << endl;
		APP_LOG(LOG_LEVEL_ERROR)<<"CTP日盘初始化失败："<<err;
		return false;
	}
	cout << "CTP日盘初始化成功."<<endl;
	return true;
}

bool AutoRun::DoDayClose(){
	if (NULL != ctp_client_)
	{
		cout << "退出接口...";
		ctp_client_->DoAfterMarket(true);
		cout << "     退出成功." << endl;
	}
	return true;
}

bool AutoRun::DoNightOpen(){
	string err;
	if (!ctp_client_->StartUp(false, err))
	{
		cout << "夜盘初始化失败:" << err << endl;
		APP_LOG(LOG_LEVEL_ERROR)<<"CTP夜盘初始化失败:"<<err;
		return false;
	}
	cout << "CTP夜盘初始化成功."<<endl;
	return true;
}

bool AutoRun::DoNightClose(){
	if (NULL != ctp_client_)
	{
		cout << "退出接口...";
		ctp_client_->DoAfterMarket(false);
		cout << "     退出成功." << endl;
	}
	return true;
}

