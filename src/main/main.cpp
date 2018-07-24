
#include <boost/thread/thread.hpp>
#include <iostream>
#include <stdarg.h>
#include "QtCore/QFile"
#include "QtCore/QDebug"
#include "QtCore/QCoreApplication"
#include "QtCore/QDir"
#include "common/AppLog.h"
#include "common/DateTime.h"
#include "common/Directory.h"
#include "common/Thread.h"
#include "network/Timer.h"
#include "strategy/Strategy.h"
#include "datalib/ReadWriteDataFile.h"
#include "strategy/AutoRun.h"
#include "datalib/SimpleMath.h"

void tick2csv(QString fromDir, QString toDir, QString file) {
	
	QString fromFile = fromDir+"/"+file+".data";
	std::deque<FutureTick> deq_datas;
	ReadDatasWithNum(deq_datas, fromFile.toLocal8Bit().constData());
	if(deq_datas.empty()) return;
	QString toFile = toDir+"/"+file+".csv";
	FILE *fp = fopen(toFile.toLocal8Bit().constData(), "wb+");
	string title = "Symbol,DateTime,LastPrice,Volume,Amount,OpenInterest,AskPrice,BidPrice,AskVolume,BidVolume\n";
	fwrite(title.c_str(), title.size(), 1, fp);
	for (std::deque<FutureTick>::iterator iter = deq_datas.begin(); iter != deq_datas.end(); ++iter) {			
		QString content = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10\n")
			.arg(iter->symbol.instrument).arg(iter->date_time.Str().c_str()).arg(iter->last_price)
			.arg(iter->volume).arg(iter->amount).arg(iter->open_interest).arg(iter->ask_price)
			.arg(iter->bid_price).arg(iter->ask_volume).arg(iter->bid_volume);
		string tmp = content.toLocal8Bit().constData();
		fwrite(content.toLocal8Bit().constData(), content.size(), 1, fp);
		/*char buf[512];
		sprintf(buf, "%c,%c,%s,%s,%g,%d,%g,%g,%g,%g,%g,%g,%g,%g,%g,%d,%d,%d,%g,%g\n", 
			iter->symbol.product, iter->symbol.exchange, iter->symbol.instrument, iter->date_time.Str().c_str(), iter->last_price, 
			iter->volume, iter->amount,	iter->pre_close, iter->today_open, iter->today_high, 
			iter->today_low, iter->up_limit, iter->drop_limit, iter->position, iter->pre_settlement, 
			iter->pre_open_interest, iter->bid_volume, iter->ask_volume, iter->bid_price, iter->ask_price);
		fwrite(buf, sizeof(buf), 1, fp);*/
	}
	fclose(fp);
}

void data2csv(const char *dirname){
	string data_dir = Global::Instance()->GetItsHome()+"/data/Tick/" + dirname;
	QDir sourceDir(data_dir.c_str());
	string csv_dir = data_dir + "/csv";
	QDir targetDir(csv_dir.c_str());
	if (!targetDir.exists()){ sourceDir.mkdir("csv"); }
	QFileInfoList fileInfoList2 = sourceDir.entryInfoList(); 
	foreach(QFileInfo fileInfo2, fileInfoList2){  
		if(fileInfo2.fileName() == "." || fileInfo2.fileName() == "..")  
			continue; 
		QString file = fileInfo2.fileName().remove(".data");
		tick2csv(sourceDir.absolutePath(), targetDir.absolutePath(), file);
	}
}

void cal_trade_list(){
	string trade_file = Global::Instance()->GetItsHome()+"/data/TradeList.txt";
	ifstream ifs(trade_file.c_str());
	while (!ifs.eof()) {
		char buf[128] = {0};
		ifs.getline(buf, 128);
		if (strlen(buf) == 0) break;
		TradeData trade;
		trade.FromStr(buf);
	}
	ifs.close();	
}

//#define TESTING

int main(int argc,char* argv[])
{
#ifdef TESTING	
	QCoreApplication app(argc, argv);
	string err;
	Strategy *strategy = new Strategy();
	strategy->Init(err);
	Thread::Sleep(10000);
	strategy->Denit();
	return app.exec();
#else
	QCoreApplication app(argc, argv);
	AutoRun *task = new AutoRun();
	app.exec();
#endif
}

