
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


void get_table()
{	
	MySecurityInfoSpi *future = new MySecurityInfoSpi(PRODUCT_FUTURE);
	future->init();
	Thread::Sleep(1000000000);
}

void get_data() {
	CtpDataServer *ctp_ser_ = new CtpDataServer();
	string err;
	ctp_ser_->StartUp(true, err);
	Thread::Sleep(1000000000);
}

int main(int argc,char* argv[])
{
	QCoreApplication app(argc, argv);

	QString q_path = QString::fromLocal8Bit("a/b/c");
	QStringList tag_names = q_path.split('/', QString::SkipEmptyParts);
	for (int i=0; i < tag_names.size(); ++i) {
		qDebug() << tag_names[i];
	}

	//SetItsHome();

	boost::thread thrd(&get_data);
	thrd.join();


	return app.exec();
}
