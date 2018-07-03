
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"
#include "QtCore/QCoreApplication"
#include "common/AppLog.h"
#include "common/DateTime.h"
#include "common/Directory.h"
#include "common/Thread.h"
#include "MySecurityInfoSpi.h"
#include "CtpDataServer.h"
#include "network/Timer.h"
#include "strategy/Strategy.h"

void get_table()
{	
	MySecurityInfoSpi *future = new MySecurityInfoSpi(PRODUCT_FUTURE);
	if (future->init()) {
		Thread::Sleep(10000);
		future->denit();
	}
	
}

void get_data() {
	CtpDataServer *ctp_ser_ = new CtpDataServer();
	string err;
	ctp_ser_->StartUp(true, err);
}

class MyTimerSpi : public TimerSpi {
	virtual void OnTimer() {
		APP_LOG(LOG_LEVEL_INFO) << "OnTimer";
	}
	TimerApi *timer_;
public:
	MyTimerSpi(){
		timer_ = new TimerApi(1000, this);
		timer_->Start(3000);
	}
};

int main(int argc,char* argv[])
{
	QCoreApplication app(argc, argv);
	string err;
	//get_table();
	//boost::thread thrd2(&get_data);
	//MyTimerSpi spi;
	Strategy *strategy = new Strategy();
	strategy->Init(err);


	return app.exec();
}
