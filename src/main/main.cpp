
#include <boost/thread/thread.hpp>
#include <iostream>
#include "QtCore/QFile"
#include "QtCore/QDebug"
#include "common/AppLog.h"
#include "common/DateTime.h"
#include "common/Directory.h"
#include "common/Thread.h"
#include "MySecurityInfoSpi.h"


void get_table()
{	
	SetItsHome();
	MySecurityInfoSpi *future = new MySecurityInfoSpi(PRODUCT_FUTURE);
	future->init();
	Thread::Sleep(10000);
}

int main(int argc,char* argv[])
{
	boost::thread thrd(&get_table);
	thrd.join();


	return 0;
}
