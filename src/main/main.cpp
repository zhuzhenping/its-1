
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
