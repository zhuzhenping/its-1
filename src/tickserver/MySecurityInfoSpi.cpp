#include "MySecurityInfoSpi.h"
#include "common/WinAdapteLinux.h"
#include "common/XmlConfig.h"
#include "common/DateTime.h"
#include "datalib/SymbolChanger.h"
#include "datalib/DataServerStruct.h"
//#include "datalib/ReadWriteDataFile.h"

using namespace std;

//namespace itstation {
  
SymbolChineseName::SymbolChineseName()
{
	std::string its_home = Global::Instance()->GetItsHome();
	std::string conf_path = Global::Instance()->GetConfigDir() + "ChineseName.xml";
	XmlConfig xml_config(conf_path);
	xml_config.Load();
	XmlNodeVec nodes = xml_config.FindChileren("ChiName", "Node");
	QString key = "ChiName/";
	for (int i=0; i < nodes.size(); ++i)
	{
		name_dict_[nodes[i].GetValue("Product")] = nodes[i].GetValue("Name");
	}

	/*std::string its_home = Global::Instance()->its_home;
	std::string conf_path = its_home + "/data/FutureChiName.ini";
	settings_ = new QSettings(conf_path.c_str(), QSettings::IniFormat);
	settings_->setIniCodec("UTF-8");

	conf_path = its_home + "/cfg/ChineseName.xml";	
	XmlConfig xml_config(conf_path);
	xml_config.Load();
	XmlNodeVec nodes = xml_config.FindChileren("ChiName", "Node");
	QString key = "ChiName/";
	for (int i=0; i < nodes.size(); ++i)
	{
		
		settings_->setValue(key + nodes[i].GetValue("Product").c_str(), nodes[i].GetValue("Name").c_str());
	}*/

}
std::string SymbolChineseName::ChiName(const std::string& inst, const std::string& name)
{
	std::string eng_name = PrefixStr(inst);
	std::string code = CodeStr(inst);

	std::string chi_name = name_dict_[eng_name];
	if (chi_name != "")
	{
		return chi_name + code;
	}
	else
	{
		std::string chi_name = PrefixStr(name);
		name_dict_[eng_name] = chi_name;
		return name;
	}
}

std::string SymbolChineseName::PrefixStr(const std::string& name)
{
	QString str(name.c_str());
	for (int i=str.size() - 1; i >= 0; --i)
	{
		if (!str[i].isDigit())
		{
			QString pre_str = str.left(i + 1);
			return pre_str.toLocal8Bit().constData();
		}
	}
	return "";
}

std::string SymbolChineseName::CodeStr(const std::string& name)
{
	QString str(name.c_str());
	for (int i=str.size() - 1; i >= 0; --i)
	{
		if (!str[i].isDigit())
		{
			QString after_str = str.right(str.size() - i - 1);
			return after_str.toLocal8Bit().constData();
		}
	}
	return "";
}

void MyMarketTradeCallback::OnError(const int request_id, const std::string& error_msg) 
{
	APP_LOG(LOG_LEVEL_ERROR) << error_msg;
}

void MyMarketTradeCallback::OnDisconnect(const std::string& reson){
	APP_LOG(LOG_LEVEL_WARN) << reson;
}

void MyMarketTradeCallback::OnConnect() {
	APP_LOG(LOG_LEVEL_INFO) << "连接成功.";
}

MySecurityInfoSpi::MySecurityInfoSpi(ProductIdType product)
	: /*TimedStateTaskManager(), */product_(product), fp_(NULL)
{	

	market_trade_callback = new MyMarketTradeCallback();
	if (product == PRODUCT_STOCK)
		trade_api = MarketTradeApiFactory::CreateMarketTradeApi(MarketTradeApiFactory::kJZStockApi);
	else if (product == PRODUCT_FUTURE)
		trade_api = MarketTradeApiFactory::CreateMarketTradeApi(MarketTradeApiFactory::kCtpFutureApi);
	else
		assert(false);

	std::string conf_path = Global::Instance()->GetConfigDir() + "config.xml";
	trade_time_path = Global::Instance()->GetConfigDir() + "TradingTime.xml";
	if (!QFile::exists(conf_path.c_str()) || !QFile::exists(trade_time_path.c_str()))
	{
		APP_LOG(LOG_LEVEL_ERROR) << conf_path << "或." << trade_time_path << "不存在.";
		return;
	}
	else
	{
		XmlConfig config(conf_path);
		XmlConfig config2(trade_time_path);
		if (!config.Load() || !config2.Load()) { 
			APP_LOG(LOG_LEVEL_ERROR) << "加在配置文件失败";
			return;
		}

		XmlNode node = config.FindNode("CtpTradeAccount");	
		
		broker_id = node.GetValue("broker_id").c_str();
		user_id = node.GetValue("user_id").c_str();
		pwd = node.GetValue("pwd").c_str();
		front_addr = node.GetValue("front_addr").c_str();

		year = QDate::currentDate().year();
		XmlNodeVec node_vec = config2.FindChileren("Holidays");
		for (int i=0; i < node_vec.size(); ++i)
		{
			if (atoi(node_vec[i].GetValue("Year").c_str()) == year)
			{
				holidays = QString(node_vec[i].GetValue("Days").c_str()).split(',');
				break;
			}
		}
	}

	/*day_open_time_ = Time(8, 20);
	night_open_time_ = Time(20, 20);
	SetInterval(6000);
	StartTimer();*/
}

MySecurityInfoSpi::~MySecurityInfoSpi() 
{
	delete market_trade_callback;
	delete trade_api;
}
/*

bool MySecurityInfoSpi::DoDayOpen(){
	APP_LOG(LOG_LEVEL_INFO) << "DoDayOpen\n";
	return SlotTimeOut() ;
}

bool MySecurityInfoSpi::DoNightOpen(){
	APP_LOG(LOG_LEVEL_INFO) << "DoNightOpen\n";
	return SlotTimeOut() ;
}
*/

//bool MySecurityInfoSpi::SlotTimeOut() {	
bool MySecurityInfoSpi::Init() {
	DateTime now(NULL);
	
	std::string err;
	if (!trade_api->Init(front_addr.toLocal8Bit().constData(), market_trade_callback, err)) 
	{
		APP_LOG(LOG_LEVEL_ERROR) << "Init error :" << err;
		trade_api->Deinit();
		return false;
	}

	if (!trade_api->Login(broker_id.toLocal8Bit().constData(), user_id.toLocal8Bit().constData(), pwd.toLocal8Bit().constData(), err)) 
	{
		APP_LOG(LOG_LEVEL_ERROR) << "Login error :" << err;
		trade_api->Deinit();
		return false;
	}
	Thread::Sleep(1000);
	if(!trade_api->InitPreTrade(err)){
		APP_LOG(LOG_LEVEL_ERROR) << "InitPreTrade error :" << err;
		trade_api->Deinit();
		return false;
	}

	trade_api->SetSecurityInfoSpi(this);
	if (!trade_api->QryInstrument(err)) 
	{
		APP_LOG(LOG_LEVEL_ERROR) << "QryInstrument error :" << err;
		return false;
	}
	
		
	return true;
}



void MySecurityInfoSpi::WriteFile()
{
	
	if (NULL == fp_)
	{
		std::string its_home = getenv("ITS_HOME");
		std::string path = its_home + "/data/FutureTable.txt";
		
		fp_ = fopen(path.c_str(), "w");
		if (NULL == fp_) 
		{
			APP_LOG(LOG_LEVEL_ERROR) << "open file error : " << path;
			return;
		}
	}
	
	
	SymbolChineseName sym_chi;
	
	for (map<string, QSharedPointer<BaseInstrumentInfo> >::iterator iter = infos_.begin(); iter != infos_.end(); ++iter)
	{
		if (product_ == PRODUCT_FUTURE) {
			std::string chi_name = sym_chi.ChiName(iter->second->symbol.instrument, iter->second->symbol.name);
			strcpy(iter->second->symbol.name, chi_name.c_str());
		}

		string str = iter->second->ToStr();
		fwrite(str.c_str(), str.size(), 1, fp_);
		fwrite("\n", 1, 1, fp_);
	}


	if (NULL != fp_)
	{
		APP_LOG(LOG_LEVEL_INFO) << "write FutureTable.txt success";
		fclose(fp_);
		fp_ = NULL;
	}
	
	
	
}


