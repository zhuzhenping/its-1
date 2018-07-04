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
	AutoRun *task = new AutoRun();
	task->StartUp();
	app.exec();
}