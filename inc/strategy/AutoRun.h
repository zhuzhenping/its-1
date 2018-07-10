#ifndef AUTO_RUN_H_
#define AUTO_RUN_H_

#include "network/MarketTask.h"
#include "common/DateTime.h"
#include "strategy/Strategy.h"

class STRATEGY_API AutoRun : public TimedStateTaskManager {
public:
	AutoRun();
	virtual ~AutoRun();

private:
	virtual bool DoDayOpen();
	virtual bool DoDayClose();
	virtual bool DoNightOpen();
	virtual bool DoNightClose();

	Strategy *strategy_;

};


#endif

