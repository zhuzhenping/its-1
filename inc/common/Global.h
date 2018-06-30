#ifndef EYEGLE_COMMON_GLOBAL_H_
#define EYEGLE_COMMON_GLOBAL_H_

#include <string>

#ifdef WIN32

//Common宏.
#ifdef COMMON_EXPORTS
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //COMMON_EXPORTS


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

#else	//LINUX平台，定义导出宏为空.
#define COMMON_API
#define DATALIB_API
#define CTP_API
#define ACCOUNT_API

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