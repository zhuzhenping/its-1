
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"
#include "common/AppLog.h"
#include "common/DateTime.h"



void hello()
{	
	QFile file("");
	if (!file.exists()) {
		qDebug() << "file not exist";
	}
	DateTime now(NULL);
	APP_LOG(LOG_LEVEL_INFO) << now.Str();
}

int main(int argc,char* argv[])
{
	boost::thread thrd(&hello);
	thrd.join();
	return 0;
}
