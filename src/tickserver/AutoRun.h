#ifndef DATA_SERVER_BACK_TASK_H_
#define DATA_SERVER_BACK_TASK_H_

#include "datalib/MarketTask.h"
#include "CtpClient.h"
#include "common/DateTime.h"
#include "MySecurityInfoSpi.h"
class QDate;

//namespace itstation {

class AutoRun : public TimedStateTaskManager {
public:
	AutoRun();
	virtual ~AutoRun();

	void StartUp();

private:
	virtual bool DoDayOpen();
	virtual bool DoDayClose();
	virtual bool DoNightOpen();
	virtual bool DoNightClose();

	CtpClient* ctp_client_;
	MySecurityInfoSpi *instrument_table_;
};


#endif

