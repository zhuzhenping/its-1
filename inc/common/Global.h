#ifndef _COMMON_GLOBAL_H_
#define _COMMON_GLOBAL_H_

#include <string>
using namespace std;
#ifdef WIN32

//Common宏.
#ifdef COMMON_EXPORTS
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //COMMON_EXPORTS


#ifdef NETWORK_EXPORTS
#define NETWORK_API  __declspec(dllexport)
#else 
#define NETWORK_API  __declspec(dllimport)
#endif  //NETWORK_EXPORTS

#ifdef DATALIB_EXPORTS
#define DATALIB_API __declspec(dllexport)
#else
#define DATALIB_API  __declspec(dllimport)
#endif //DATALIB_EXPORTS

#ifdef CTP_EXPORTS
#define CTP_API __declspec(dllexport)
#else
#define CTP_API  __declspec(dllimport)
#endif //CTP_EXPORTS

#ifdef ACCOUNT_EXPORTS
#define ACCOUNT_API __declspec(dllexport)
#else
#define ACCOUNT_API  __declspec(dllimport)
#endif // ACCOUNT_EXPORTS

#ifdef STRATEGY_EXPORTS
#define STRATEGY_API __declspec(dllexport)
#else
#define STRATEGY_API  __declspec(dllimport)
#endif

#else	//LINUX平台，定义导出宏为空.
#define COMMON_API
#define NETWORK_API
#define DATALIB_API
#define CTP_API
#define ACCOUNT_API
#define STRATEGY_API

#define nullptr NULL
#endif  //WIN32

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
	std::string its_home;

private:
	Global();
	void Init();

	static Global* m_instance;
	std::string m_app_name;
};

#endif