#ifndef _COMMON_GLOBAL_H_
#define _COMMON_GLOBAL_H_

#include <string>

#ifdef WIN32

//Common宏.
#ifdef COMMON_EXPORT
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //COMMON_EXPORT

//
#ifdef NETWORK_EXPORT
#define NETWORK_API  __declspec(dllexport)
#else 
#define NETWORK_API  __declspec(dllimport)
#endif 

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

#ifdef DATALIB_EXPORT
#define DATALIB_API  __declspec(dllexport)
#else 
#define DATALIB_API  __declspec(dllimport)
#endif 


#else	//LINUX平台，定义导出宏为空.
#define COMMON_API
#define NETWORK_API
#define MARKET_TRADE_API
#define MARKET_DATA_API
#define DATALIB_API

#define nullptr NULL
#endif  //WIN32

namespace zhongan {

class COMMON_API Global {
public:
	static Global* GetInstance();
	// 取配置文件夹路径.
	std::string GetConfigDir() const;
	// 取plugins路径.
	std::string GetPluginsDir() const;
	// 获取应用程序名.
	std::string GetAppName() const;

public:
	std::string zhongan_home;

private:
	Global();
	void Init();

	static Global* m_instance;
	std::string m_app_name;
};

}
#endif