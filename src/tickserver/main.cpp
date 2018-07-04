#include <iostream>
#include <QtCore/QCoreApplication>
#include "common/Directory.h"
#include "common/AppLog.h"
#include "AutoRun.h"

using namespace std;
//using namespace itstation;

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	APP_LOG(LOG_LEVEL_INFO) << "TickServer 开始 --- ";
	AutoRun *task = new AutoRun();
	task->StartUp();
	app.exec();
}