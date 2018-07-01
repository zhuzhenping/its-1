#include "ctp/MarketDataApi.h"
#include <iostream>

////namespace itstation {
////namespace itstation {

MarketDataApi::MarketDataApi()
	: spi_(NULL), m_succed_connect(false), m_succed_login(false), holidays_(NULL)
{

}

MarketDataApi::~MarketDataApi() {}

bool MarketDataApi::TimeWait(int sec)
{
	if (sec <= 0) { return false; }

	Locker lock(&wait_mutex_);
	return wait_cond_.TimedWait(&wait_mutex_, sec);
}

void MarketDataApi::ReleaseWait()
{
	wait_cond_.Signal();
}

