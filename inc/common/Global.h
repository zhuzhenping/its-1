#ifndef EYEGLE_COMMON_GLOBAL_H_
#define EYEGLE_COMMON_GLOBAL_H_

#include <string>

#ifdef WIN32

//Common宏.
#ifdef COMMON_EXPORTS
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //COMMON_EXPORT

//MarketTrade宏.
#ifdef MARKET_TRADE_EXPORT
#define MARKET_TRADE_API  __declspec(dllexport)
#else 
#define MARKET_TRADE_API  __declspec(dllimport)
#endif 

//MarketData宏.
#ifdef MARKET_DATA_EXPORT
#define MARKET_DATA_API  __declspec(dllexport)
#else 
#define MARKET_DATA_API  __declspec(dllimport)
#endif 

//STRATEGY宏.
#ifdef STRATEGY_EXPORT
#define STRATEGY_API  __declspec(dllexport)
#else 
#define STRATEGY_API  __declspec(dllimport)
#endif 

//
#ifdef Server_EXPORT
#define Server_API  __declspec(dllexport)
#else 
#define Server_API  __declspec(dllimport)
#endif 

//PositionManager宏.
#ifdef POSITION_MANAGER_EXPORT
#define POSITION_MANAGER_API  __declspec(dllexport)
#else 
#define POSITION_MANAGER_API  __declspec(dllimport)
#endif 

//DATA_API_LIB宏.
#ifdef DATA_API_LIB_EXPORT
#define DATA_API_LIB_API  __declspec(dllexport)
#else 
#define DATA_API_LIB_API  __declspec(dllimport)
#endif 

//ACCOUNT_EXPORT宏.
#ifdef ACCOUNT_EXPORT
#define ACCOUNT_EXPORT_API  __declspec(dllexport)
#else 
#define ACCOUNT_EXPORT_API  __declspec(dllimport)
#endif 

#ifdef HIS_DATA_LIB_EXPORT
#define HIS_DATA_LIB_API //__declspec(dllexport)
#else
#define HIS_DATA_LIB_API  //__declspec(dllimport)
#endif

#ifdef DATALIB_EXPORT
#define DATALIB_API __declspec(dllexport)
#else
#define DATALIB_API  __declspec(dllimport)
#endif

//UI_FRAME_EXPORT宏.
#ifdef UI_FRAME_EXPORT
#define UI_FRAME_API  __declspec(dllexport)
#else 
#define UI_FRAME_API  __declspec(dllimport)
#endif 

//BASE_WIDGET_LIB宏.
#ifdef BASE_WIDGET_LIB_EXPORT
#define BASE_WIDGET_LIB_API  __declspec(dllexport)
#else 
#define BASE_WIDGET_LIB_API  __declspec(dllimport)
#endif 

//ACCOUNT_CENTER宏.
#ifdef ACCOUNT_CENTER_EXPORT
#define ACCOUNT_CENTER_API  __declspec(dllexport)
#else 
#define ACCOUNT_CENTER_API  __declspec(dllimport)
#endif 

//DATA_CLIENT宏.
#ifdef DATA_CLIENT_EXPORT
#define DATA_CLIENT_API  __declspec(dllexport)
#else 
#define DATA_CLIENT_API  __declspec(dllimport)
#endif 

//WIN_DBG_ENGINE宏.
#ifdef WIN_DBG_ENGINE_EXPORT
#define WIN_DBG_ENGINE_API  __declspec(dllexport)
#else 
#define WIN_DBG_ENGINE_API  __declspec(dllimport)
#endif 

#else	//LINUX平台，定义导出宏为空.
#define COMMON_API
#define MARKET_TRADE_API
#define MARKET_DATA_API
#define MIDWARE_API
#define DB_API
#define POSITION_MANAGER_API
#define ACCOUNT_EXPORT_API
#define HIS_DATA_LIB_API
#define DATA_API_LIB_API
#define UI_FRAME_API
#define BASE_WIDGET_LIB_API
#define ACCOUNT_CENTER_API
#define DATA_CLIENT_API
#define WIN_DBG_ENGINE_API
#define nullptr NULL
#endif  //WIN32

class COMMON_API Global {
public:
	static Global* GetInstance();
	std::string GetConfigDir() const;
	void SetAppConfigFileName(const std::string& file_name);
	std::string GetAppConfigPath() const;
	void SetAppName(const std::string& name);
	std::string GetAppName() const;

public:
	std::string its_home;

private:
	Global();
	void Init();

	static Global* m_instance;
	std::string m_app_conf_file_name;
	std::string m_app_name;
};

#endif