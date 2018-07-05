
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"
#include "QtCore/QCoreApplication"
#include "common/AppLog.h"
#include "common/DateTime.h"
#include "common/Directory.h"
#include "common/Thread.h"
#include "network/Timer.h"
#include "strategy/Strategy.h"

class MyTimerSpi : public TimerSpi {
public:
	virtual void OnTimer(){
		APP_LOG_DBG<<"1";
		Thread::Sleep(3000);
	}
	MyTimerSpi(){
		api = new TimerApi(10000, this);
	}
	TimerApi *api;
};
int main(int argc,char* argv[])
{
	QCoreApplication app(argc, argv);
	string err;
	//get_table();
	//boost::thread thrd2(&get_data);
	//MyTimerSpi spi;
	/*Strategy *strategy = new Strategy();
	strategy->Init(err);*/

	MyTimerSpi spi;
	spi.api->Start(DateTime::ToNextMin());


	return app.exec();
}
