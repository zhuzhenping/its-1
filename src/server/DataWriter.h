#ifndef DATA_SERVER_DATA_HIS_DATA_FILE_H_
#define DATA_SERVER_DATA_HIS_DATA_FILE_H_

#include <deque>
#include <iostream>
#include "common/SpinLock.h"
#include "DataLib/SearchIndex.h"
#include "DataLib/Symbol2T.h"
#include "network/Timer.h"

class DataObj : public TimerSpi
{
	enum {
		TICK_QUEUE_LEN = 100
	};
public:
	DataObj(const Symbol& symbol, bool is_night);
	~DataObj();

	// update kline
	void PushTick(FutureTick* tick);

	void DoAfterMarket();//收盘了将内存里的数据写入文件.

	void SetNightFlag(bool flag) { is_night_ = flag; }

private:
	void SaveTick();
	void SaveMinKline();
	void MakeDataDir();

	virtual void OnTimer();

private:
	SpinLock mutex_; 
	std::vector<FutureKline> min_klines_;

	int cur_pos_;
	FutureTick* ticks_[TICK_QUEUE_LEN];
	SpinLock ticks_mutex_; 

	Symbol symbol_;
	FutureKline pre_kline_;

	std::string tick_path_;
	std::string min_path_;
	std::string day_path_;

	int tick_size_;

	bool is_night_;
};


typedef Symbol2T<DataObj> Symbol2DataObj;

// write tick、kline to disk、socket
class DataWriter
{
public:
	DataWriter();
	~DataWriter();


	bool Init(std::string& err, bool is_night = false);
	void Denit();

	void PushTick(FutureTick* tick);

	void DoAfterMarket();

private:
	bool InitContainer(std::string& err);

	DataObj* GetDataObj(const Symbol& sym);

private:
	Symbol2DataObj* data_objs_;

	bool is_init_;
	SpinLock mutex_; 
	bool is_night_;
};

#endif

