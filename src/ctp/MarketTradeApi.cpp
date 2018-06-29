#include "ctp/MarketTradeApi.h"
#include <QtCore/QFile>
#include <QtCore/QSettings>

//namespace itstation {
//namespace marketapi {

TradeApi::TradeApi() 
	: spi_(NULL)
	, sec_info_spi_(NULL)
	, m_succed_connect(false)
	, m_succed_login(false)
{
	/*std::string its_home = getenv("ITS_HOME");
	std::string conf_path = its_home + "\\config\\config.ini";
	if (!QFile::exists(conf_path.c_str())) {
		assert(false);
	}
	config_settings_ = new QSettings(conf_path.c_str(), QSettings::IniFormat);*/
}

TradeApi::~TradeApi()
{
	//delete config_settings_; config_settings_ = nullptr;
}

bool TradeApi::TimeWait(int sec)
{
	if (sec <= 0) { return false; }

	MutexLocker lock(&wait_mutex_);
	return wait_cond_.TimedWait(&wait_mutex_, sec);
}

void TradeApi::ReleaseWait()
{
	wait_cond_.Signal();
}

