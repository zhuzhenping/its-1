#include <iostream>
#include <QtCore/QCoreApplication>
#include "common/Directory.h"
#include "common/AppLog.h"
#include "AutoRun.h"
#include "common/SimpleDateTime.h"
#include "Server.h"

using namespace std;

#define TESTING


Server *g_server_ = NULL; // tick server and kline server

int main(int argc, char *argv[])
{
	XmlConfig config(Global::Instance()->GetConfigFile());
	if (!config.Load()) return -1;
	XmlNode node = config.FindNode("DataServer");
	int TcpServer_port = atoi(node.GetValue("port").c_str());
	g_server_ = new Server(TcpServer_port);

#ifdef TESTING
	QCoreApplication app(argc, argv);	

	string err;
	/*MySecurityInfoSpi *instrument_table_;
	instrument_table_ = new MySecurityInfoSpi(PRODUCT_FUTURE);
	instrument_table_->Init();
	Thread::Sleep(5000);
	instrument_table_->Denit();*/
	CtpClient * ctp_client_ = new CtpClient;
	ctp_client_->Init(true, err);
	Thread::Sleep(5000);
	ctp_client_->Denit();
	app.exec();
#else
	QCoreApplication app(argc, argv);
	AutoRun *task = new AutoRun();
	app.exec();
#endif
}