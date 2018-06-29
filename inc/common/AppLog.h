/*!
 * \brief       异步打印日志.
 * \author      吴典@众安科技虚拟实验室.
 * \date        -
 *
 * \usage
 * APP_LOG(LOG_LEVEL_ERROR) << "错误内容";
 *
 */

#ifndef _COMMON_APP_LOG_H_  
#define _COMMON_APP_LOG_H_  

#include <memory.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include "common/QueueBuffer.h"
#include "common/SpendTime.h"

using namespace std;

extern SpendTime timer;

#define APP_LOG(LEVEL) zhongan::common::AppLogFinisher() = zhongan::common::AppLogInput(zhongan::common::LEVEL, __FILE__, __LINE__)

//#ifndef _DEBUG
//#define _DEBUG
//#endif

#ifdef _DEBUG
#define APP_LOG_DBG APP_LOG(LOG_LEVEL_DEBUG)
#else
struct LogEmptyInput
{
	LogEmptyInput& operator<<(const string& msg){ return *this; }
	LogEmptyInput& operator<<(const char* msg){ return *this; }
	LogEmptyInput& operator<<(char msg){ return *this; }
	LogEmptyInput& operator<<(int msg){ return *this; }
	LogEmptyInput& operator<<(unsigned int msg){ return *this; }
	LogEmptyInput& operator<<(double msg){ return *this; }
	LogEmptyInput& operator<<(long msg){ return *this; }
	LogEmptyInput& operator<<(long long msg){ return *this; }

	static COMMON_API LogEmptyInput instance;
};
#define APP_LOG_DBG LogEmptyInput::instance
#endif

namespace zhongan {
namespace common {

/*
 * @brief LogLevel 日志层级枚举值.
 * @note 关闭层级不会打印任何东西；日志当前层表示只打印当前及以上的层级的信息.
 * 如警告层级只打印警告和错误的日志、调试层级打印所有日志信息.
 */
typedef enum {
	LOG_LEVEL_OFF         = 0,	/**< 关闭层级 */
	LOG_LEVEL_ERROR       = 1,	/**< 错误层级 */
	LOG_LEVEL_WARN        = 2,	/**< 警告层级 */
	LOG_LEVEL_INFO		  = 3,	/**< 信息层级 */
	LOG_LEVEL_SUCCESS     = 4,	/**< 成功层级 */
	LOG_LEVEL_DEBUG		  = 5	/**< 调试层级 */
} LogLevel;

struct LogData 
{
	LogLevel level;
	char file_name[128];
	int line;
	char message[128];

	LogData() : level(LOG_LEVEL_INFO), line(0) 
	{
		memset(file_name, 0, 128);
		memset(message, 0, 128);
	}

	LogData(LogLevel l, const char* filename, int line_, const char* msg) : level(l), line(line_)
	{
		memcpy(file_name, filename, 128);
		memcpy(message, msg, 128);
	}
};

class COMMON_API AppLog : public QueueBuffer<LogData, 50>
{
public:
	enum{
		DEFAULT_BUF_LEN			= 50,				/**< 默认缓冲区的长度 */
		DEFAULT_LOG_MAX_SIZE	= 1000 * 1024,		/**< 默认日志容量,1M */
		DEFAULT_LOG_MAX_NUM		= 10				/**< 默认日志数量 */
	};

public:
	~AppLog();
	static AppLog* GetInstance();

	void set_level(LogLevel level) { m_level = level; }
	LogLevel get_level() const { return m_level; }
	void set_max_size(int size) { m_max_size = size > 0 ? size : m_max_size; }
	int get_max_size() const { return m_max_size; }
	void set_max_num(int num) { m_max_num = num > 0 ? num : m_max_num; }
	int get_max_num() const { return m_max_num;}
	
private:
	AppLog();
	virtual bool Comsume(const LogData& val);
	void InitLog();

	LogLevel StringToLevel(const char* value);
	const char*  LevelToString(LogLevel level);
	const char* GetNowTime();
	void OpenLogFile();
	//如果文件大小超过设置，则滚动重命名日志文件.
	void RollLogFile();

private:
	static AppLog* m_instance;

	LogLevel m_level;			/**< 日志层级 */
	int m_max_size;				/**< 日志最大容量 */
	int m_max_num;				/**< 循环日志的最大个数，超过这个数则自动删除最早的日志文件 */
	FILE* m_file;				/**< 日志文件对象 */
	bool m_is_running;			/**< 日志线程是否已经在运行 */

	string m_file_path;
	string m_err_file_path;
	bool m_append_console;

	static struct LastExit 
	{
		~LastExit()
		{
			if (AppLog::m_instance)
			{
				delete AppLog::m_instance;
				AppLog::m_instance = NULL;
			}
		}
	} last_exit;
};

class AppLogInput
{    
private:
	char m_file_name[128];
	int m_line;
	char m_message[128];
	LogLevel m_level;
public:
	friend class AppLogFinisher;
	AppLogInput(LogLevel l, const char* filename, int line) : m_level(l), m_line(line)
	{  
		memcpy(m_file_name, filename, 128);
		memset(m_message, 0, 128);
	};

	AppLogInput& operator<<(const string& msg) { 
		strcat(m_message, msg.c_str()); 
		return *this; 
	}

	AppLogInput& operator<<(const char* msg) { 
		strcat(m_message, msg); 
		return *this; 
	}

	AppLogInput& operator<<(char c) 
	{ 
		sprintf(m_message, "%s%c", m_message, c);
		return *this;
	}

	AppLogInput& operator<<(int n) {
		sprintf(m_message, "%s%d", m_message, n);
		return *this;
	}

	AppLogInput& operator<<(unsigned int n) {
		sprintf(m_message, "%s%d", m_message, n);
		return *this;
	}

	AppLogInput& operator<<(double d) {
		sprintf(m_message, "%s%f", m_message, d);
		return *this;
	}

	AppLogInput& operator<<(long l) {
		sprintf(m_message, "%s%ld", m_message, l);
		return *this;
	}

	AppLogInput& operator<<(long long l) {
		sprintf(m_message, "%s%ld", m_message, l);
		return *this;
	}
};

class AppLogFinisher
{
public:
	void operator=(AppLogInput& input) {
		AppLog::GetInstance()->Push(LogData(input.m_level, input.m_file_name, input.m_line, input.m_message));
	};
};

}
}

#endif