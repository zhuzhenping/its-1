
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"
#include "common/AppLog.h"
#include "common/DateTime.h"

void hello()
{
	QFile file("/home/wd/its/src/main/main.cpp");
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug()<<"Can't open the file!"<<endl;
		file.close();
	}
	
	DateTime now(NULL);
	APP_LOG(LOG_LEVEL_INFO) << now.Str();
	
}

int main(int argc,char* argv[])
{
	APP_LOG(LOG_LEVEL_INFO) << "begin";
	boost::thread thrd(&hello);
	thrd.join();
	return 0;
}
