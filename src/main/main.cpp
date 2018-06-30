
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"
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
	//SetItsHome();

	boost::thread thrd(&get_data);
	thrd.join();


	return 0;
}
