#ifndef DATA_SERVER_DATA_HIS_DATA_FILE_H_
#define DATA_SERVER_DATA_HIS_DATA_FILE_H_

#include <vector>
#include <iostream>
#include "common/SpinLock.h"
#include "datalib/SearchIndex.h"
#include "datalib/Symbol2T.h"
#include "network/Timer.h"
#include "network/Server.h"

class DataObj
{
	enum {
		TICK_QUEUE_LEN = 100
	};
public:
	DataObj(const Symbol& symbol, bool is_night);
	~DataObj();

	void GetKlines(std::vector<FutureKline>&);

	// update kline
	void PushTick(FutureTick* tick);

	void DoAfterMarket();//收盘了将内存里的数据写入文件.

	void SetNightFlag(bool flag) { is_night_ = flag; }

	void OnTimer(); // push kline per min

private:
	void SaveTick();
	void SaveMinKline();
	void MakeDataDir();
	void InitSectionTime();
	bool IsInSectionTime(const SimpleDateTime &);

private:
	SpinLock min_klines_mutex_; 
	std::vector<FutureKline> min_klines_;

	int cur_pos_;
	FutureTick* ticks_[TICK_QUEUE_LEN];
	SpinLock ticks_mutex_; 

	Symbol symbol_;
	bool is_first_tick_; // first tick in 1 min
	FutureKline kline_;
	SpinLock kline_mutex_; 

	std::string tick_path_;
	std::string min_path_;
	std::string day_path_;

	bool is_night_;
	std::vector<TradeSectionTime> m_section_time;
};


typedef Symbol2T<DataObj> Symbol2DataObj;

// write tick、kline to disk and tcp client
class DataWriter : public TimerSpi, public DataSpi
{
public:
	DataWriter();
	~DataWriter();
	
	bool Init(std::string& err, bool is_night = false);
	void Denit();

	void PushTick(FutureTick* tick);

	virtual void GetKlines(const Symbol& sym, std::vector<FutureKline>& );

private:
	bool InitContainer(std::string& err);
	void DoAfterMarket();
	DataObj* GetDataObj(const Symbol& sym);

	virtual void OnTimer();

private:
	Symbol2DataObj* data_objs_;
	SpinLock data_objs_mutex_; 

	bool is_init_;
	bool is_night_;


	TimerApi *timer_;
};

#endif

