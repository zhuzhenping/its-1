#include "DataWriter.h"
#include "common/Global.h"
#include "common/AppLog.h"
#include "common/SpendTime.h"
#include "common/Directory.h"
#include "datalib/SymbolChanger.h"
#include "DataLib/SymbolInfoSet.h"
#include "DataLib/ReadWriteDataFile.h"
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QDate>
#include <QtCore/QFile>



DataObj::DataObj(const Symbol& symbol, bool is_night)
	: symbol_(symbol)
	, cur_pos_(-1)
	, is_night_(is_night)
	, tick_size_(sizeof(FutureTick))
{
	MakeDataDir();
}

DataObj::~DataObj()
{
}

void DataObj::MakeDataDir(){
	SimpleDateTime now(time(NULL));
	if (is_night_) { 
		Date date(now.date.year, now.date.month, now.date.day);
		date = date.NextTradingDay();
		now.date = SimpleDate(date.year, date.month, date.day);
	}

	char tick_time_str[16] = {0};
	sprintf(tick_time_str, "/%04d%02d%02d/", now.date.year, now.date.month, now.date.day);

	tick_path_ = Global::Instance()->its_home + "/data/Tick/" + tick_time_str + symbol_.instrument + ".data";
	string folder = Global::Instance()->its_home + "/data/Minute/";
	if (!Directory::IsDirExist(folder)) Directory::MakeDir(folder);
	min_path_ = folder + symbol_.instrument + ".data";
	folder = Global::Instance()->its_home + "/data/Day/";
	if (!Directory::IsDirExist(folder)) Directory::MakeDir(folder);
	day_path_ = folder + symbol_.instrument + ".data";
}

void DataObj::PushTick(FutureTick* tick)
{
	FutureTick* cpy_tick = (FutureTick*)malloc(tick_size_);
	memcpy(cpy_tick, tick, tick_size_);

	Locker locker(&mutex_);
	ticks_[cur_pos_++] = cpy_tick;
	if (cur_pos_ == TICK_QUEUE_LEN - 1)
	{
		SaveTick();
	}
}

void DataObj::OnTimer() {

}

void DataObj::SaveMinKline() {
	FILE* min_fp = fopen(min_path_.c_str(), "ab+");
	if (min_fp == NULL) {
		APP_LOG(LOG_LEVEL_ERROR) << "open file error:" << min_path_;
	}
	else
	{
		APP_LOG(LOG_LEVEL_INFO) << "save kline " << symbol_.instrument << " in num: " << (int)min_klines_.size();
		if (PRODUCT_FUTURE == symbol_.product || PRODUCT_OPTION == symbol_.product) {
			for (int i=0; i<min_klines_.size(); ++i)
				fwrite(&min_klines_[i], sizeof(FutureKline), 1, min_fp);
		}
		else {
			for (int i=0; i<min_klines_.size(); ++i)
				fwrite(&min_klines_[i], sizeof(KlineExt1), 1, min_fp);
		}

		fclose(min_fp);
	}

	min_klines_.clear();
}


void DataObj::SaveTick()
{
	APP_LOG_DBG<<"write tick:"<<symbol_.instrument;

	Locker locker(&ticks_mutex_);

	FILE* fp = fopen(tick_path_.c_str(), "ab+");
	if (fp == NULL)
	{
		APP_LOG(LOG_LEVEL_ERROR) << "open file error" << tick_path_;
		for (int i=0; i<TICK_QUEUE_LEN; ++i)
		{
			free(ticks_[i]);
			ticks_[i] = NULL;
		}
		return;
	}

	for (int i=0; i <= cur_pos_; ++i)
		fwrite(ticks_[i], tick_size_, 1, fp);

	fclose(fp);


	for (int i=0; i <= cur_pos_; ++i)
	{
		free(ticks_[i]);
		ticks_[i] = NULL;
	}
	cur_pos_ = -1;
}

void DataObj::DoAfterMarket()
{
	Locker locker(&mutex_);
	if (min_klines_.size() == 0) { return; }

	SaveMinKline();
	SaveTick();
}

//////////////////////////////////////////////////////////////////////////////////////
DataWriter::DataWriter() 
	: is_init_(false)
	, is_night_(false)
	, data_objs_(NULL)
{
	
	data_objs_ = new Symbol2DataObj();
}

DataWriter::~DataWriter(void)
{
	//Denit();   销毁不需要执行收盘作业
	delete data_objs_;
}

bool DataWriter::Init(std::string& err, bool is_night)
{
	Locker locker(&mutex_);
	if (is_init_) { return true; }
	is_night_ = is_night;

	if (!InitContainer(err)) { return false; }

	is_init_ = true;
	return true;
}

void DataWriter::Denit()
{
	if (!is_init_) { return; }
	DoAfterMarket();

	is_init_ = false;
}

bool DataWriter::InitContainer(std::string& err)
{
	SymbolInfoSet* sym_info = SymbolInfoSet::Instance();

	if (!is_night_) 
	{
		const std::vector<Symbol>& futures = sym_info->FutureSymbols();
		for (int i=0; i<futures.size(); ++i)
		{
			DataObj** obj = data_objs_->Data(futures[i]);
			if (obj == NULL) { continue; }
			if (*obj != NULL)
			{
				(*obj)->SetNightFlag(is_night_);
			}
			else
			{
				*obj = new DataObj(futures[i], is_night_);
			}
		}
	}
	else
	{
		const std::vector<Symbol>& futures = sym_info->NightFutureSymbols();
		for (int i=0; i<futures.size(); ++i)
		{
			DataObj** obj = data_objs_->Data(futures[i]);
			if (obj == NULL) { continue; }
			*obj = new DataObj(futures[i], is_night_);
		}
	}

	return true;
}

DataObj* DataWriter::GetDataObj(const Symbol& sym)
{
	DataObj** obj = data_objs_->Data(sym);
	if (obj == NULL || *obj == NULL) { return NULL; }

	return *obj;
}

void DataWriter::PushTick(FutureTick* tick)
{
	Locker locker(&mutex_);
	if (!is_init_) { 
		APP_LOG(LOG_LEVEL_ERROR) << "has not init";
		return; 
	}

	DataObj* obj = GetDataObj(tick->symbol);
	if (NULL == obj) {
		APP_LOG(LOG_LEVEL_ERROR) << "symbol not in the table:" << tick->symbol.instrument;
		return;
	}

	
	obj->PushTick(tick);
	
}

/*
template<class T>
void DataWriter::GetTodayDatas(const Symbol& sym, std::deque<T>& result)
{
	Locker locker(&mutex_);
	if (!is_init_) { 
		APP_LOG(LOG_LEVEL_ERROR) << "has not init";
		return; 
	}

	DataObj* obj = GetDataObj(sym);
	if (NULL == obj) {
		APP_LOG(LOG_LEVEL_ERROR) << "symbol not in the table:" << sym.instrument;
		return;
	}

	obj->GetTodayDatas(result);
}*/

void WriteKline(DataObj* obj, void* rh)
{
	DataWriter* data_file = (DataWriter*)rh;
	if (NULL == data_file) { return; }

	obj->DoAfterMarket();
}

void DataWriter::DoAfterMarket()
{
	Locker locker(&mutex_);
	data_objs_->ForEach(WriteKline, this);
	

	if (!is_night_)
	{
		data_objs_->Clear();
		Thread::Sleep(500);
	}
}

