#include "DataWriter.h"
#include <stdlib.h>
#include <QtCore/QStringList>
#include <QtCore/QSettings>
#include <QtCore/QDate>
#include <QtCore/QFile>
#include "common/Global.h"
#include "common/AppLog.h"
#include "common/SpendTime.h"
#include "common/Directory.h"
#include "datalib/SymbolChanger.h"
#include "datalib/SymbolInfoSet.h"
#include "datalib/ReadWriteDataFile.h"

extern Server *g_server_; // tick server and kline server
static const int TICK_SIZE = sizeof(FutureTick);

DataObj::DataObj(const Symbol& symbol, bool is_night)
	: symbol_(symbol)
	, cur_pos_(0)
	, is_night_(is_night)
	, is_first_tick_(true)
{
	MakeDataDir();
	InitSectionTime();
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
	sprintf(tick_time_str, "%04d%02d%02d/", now.date.year, now.date.month, now.date.day);

	string folder = Global::Instance()->its_home + "/data/Tick/" + tick_time_str;
	if (!Directory::IsDirExist(folder)) Directory::MakeDir(folder);
	tick_path_ = folder + symbol_.instrument + ".data";
	folder = Global::Instance()->its_home + "/data/Minute/";
	min_path_ = folder + symbol_.instrument + ".data";
	/*folder = Global::Instance()->its_home + "/data/Day/";
	if (!Directory::IsDirExist(folder)) Directory::MakeDir(folder);
	day_path_ = folder + symbol_.instrument + ".data";*/
}

void DataObj::InitSectionTime() {
	SymbolInfoSet::Instance()->GetTradingTime(symbol_, m_section_time);
	if (m_section_time.size() == 0)
	{
		APP_LOG(LOG_LEVEL_ERROR) << symbol_.Str() << "  InitSectionTime fail";
	}
}

bool DataObj::IsInSectionTime(const SimpleDateTime &datetime){
	for (std::vector<TradeSectionTime>::iterator iter = m_section_time.begin(); iter != m_section_time.end(); ++iter){
		if (datetime.time >= iter->begin && datetime.time <= iter->end)
			return true;
	}

	char pro_str[8] = {0};
	sscanf(symbol_.instrument, "%[^0-9]", pro_str);
	if (!strcmp(pro_str,"al")||!strcmp(pro_str,"cu")||!strcmp(pro_str,"zn")||!strcmp(pro_str,"pb")||!strcmp(pro_str,"ni")||!strcmp(pro_str,"sn")||!strcmp(pro_str,"ag")||!strcmp(pro_str,"au")||!strcmp(pro_str,"sc")){
		if (datetime.time >= m_section_time.begin()->begin || datetime.time <= m_section_time.begin()->end)
			return true;
	}
	return false;
}

void DataObj::GetKlines(std::vector<FutureKline> &klines) {
	Locker lock(&min_klines_mutex_);
	klines.assign(min_klines_.begin(), min_klines_.end());
}

void DataObj::PushTick(FutureTick* tick)
{
	RunTickRsp rsp;
	rsp.tick = *tick;
	g_server_->Send(symbol_, (char*)&rsp, sizeof(RunTickRsp));

	{
		Locker lock(&kline_mutex_);
		kline_.update(tick, is_first_tick_);
		is_first_tick_ = false;
	}

	FutureTick* cpy_tick = (FutureTick*)malloc(TICK_SIZE);
	memcpy(cpy_tick, tick, TICK_SIZE);

	Locker locker(&ticks_mutex_);
	ticks_[cur_pos_++] = cpy_tick;
	if (cur_pos_ == TICK_QUEUE_LEN)
	{
		SaveTick();
		cur_pos_ = 0;
	}
}

void DataObj::OnTimer() {
	SimpleDateTime datetime(time(NULL));
	datetime.time.AddSec(1);
	if (!IsInSectionTime(datetime)) return; // not in trading time

	Locker lock(&kline_mutex_);
	{
		Locker locker(&min_klines_mutex_);
		min_klines_.push_back(kline_);
		APP_LOG(LOG_LEVEL_DEBUG) << kline_.Str();
	}
	RunKlineRsp rsp;
	rsp.kline = kline_;
	g_server_->Send(symbol_, (char*)&rsp, sizeof(RunKlineRsp));

	kline_.clear();
	is_first_tick_ = true;
}

void DataObj::SaveMinKline() {
	if (min_klines_.size() == 0) { return; }

	FILE* min_fp = fopen(min_path_.c_str(), "ab+");
	if (min_fp == NULL) {
		APP_LOG(LOG_LEVEL_ERROR) << "open file error:" << min_path_;
	}
	else
	{
		APP_LOG(LOG_LEVEL_DEBUG) << "save kline " << symbol_.instrument << " in num: " << (int)min_klines_.size();
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
	APP_LOG(LOG_LEVEL_DEBUG)<<"write tick:"<<symbol_.instrument;

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

	for (int i=0; i < TICK_QUEUE_LEN; ++i)
		fwrite(ticks_[i], TICK_SIZE, 1, fp);

	fclose(fp);


	for (int i=0; i < TICK_QUEUE_LEN; ++i)
	{
		free(ticks_[i]);
		ticks_[i] = NULL;
	}
}

void DataObj::DoAfterMarket()
{
	{
		Locker locker(&min_klines_mutex_);
		SaveMinKline();
	}

	{
		Locker locker(&ticks_mutex_);
		SaveTick();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
DataWriter::DataWriter() 
	: is_init_(false)
	, is_night_(false)
	, data_objs_(NULL)
{
	data_objs_ = new Symbol2DataObj();
	timer_ = new TimerApi(60000, this);
	g_server_->SetDataSpi(this);
}

DataWriter::~DataWriter(void)
{
	//Denit();   销毁不需要执行收盘作业
	delete data_objs_;
	delete timer_;
}

bool DataWriter::Init(std::string& err, bool is_night)
{
	Locker locker(&data_objs_mutex_);
	if (is_init_) { return true; }
	is_night_ = is_night;

	string folder = Global::Instance()->its_home + "/data/Tick/"; 
	if (!Directory::IsDirExist(folder)) Directory::MakeDir(folder);
	folder = Global::Instance()->its_home + "/data/Minute/";
	if (!Directory::IsDirExist(folder)) Directory::MakeDir(folder);

	if (!InitContainer(err)) { return false; }

	timer_->Start(DateTime::ToNextMin());
	is_init_ = true;
	return true;
}

void DataWriter::Denit()
{
	if (!is_init_) { return; }
	DoAfterMarket();
	timer_->Stop();
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
	Locker locker(&data_objs_mutex_);
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

void DataWriter::GetKlines(const Symbol& sym, std::vector<FutureKline>& klines){
	Locker locker(&data_objs_mutex_);
	if (!is_init_) return;
	DataObj* obj = GetDataObj(sym);
	if (NULL == obj) return;
	obj->GetKlines(klines);
}

void DataWriter::OnTimer() {
	Locker locker(&data_objs_mutex_);
	if (!is_init_) return;
	for (int i = 0; i < data_objs_->Size(); ++i) {
		DataObj *obj = data_objs_->Data(i);
		if (obj) obj->OnTimer();
	}
}

/*
template<class T>
void DataWriter::GetTodayDatas(const Symbol& sym, std::deque<T>& result)
{
	Locker locker(&data_objs_mutex_);
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

/*
void WriteKline(DataObj* obj, void* rh)
{
	DataWriter* data_file = (DataWriter*)rh;
	if (NULL == data_file) { return; }

	obj->DoAfterMarket();
}*/

void DataWriter::DoAfterMarket()
{
	Locker locker(&data_objs_mutex_);
	for (int i = 0; i < data_objs_->Size(); ++i) {
		data_objs_->Data(i)->DoAfterMarket();
	}
	//data_objs_->ForEach(WriteKline, this);
	

	if (!is_night_)
	{
		data_objs_->Clear();
		Thread::Sleep(500);
	}
}

