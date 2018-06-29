#include <sstream>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QCoreApplication>
#include "common/XmlConfig.h"
#include "common/AppLog.h"
#include "common/Global.h"
#include "common/Directory.h"
#include "common/StatusDefine.h"

#ifndef _DEBUG
LogEmptyInput LogEmptyInput::instance;
#endif

namespace zhongan {
namespace common {

AppLog* AppLog::m_instance = NULL;
AppLog::LastExit AppLog::last_exit;

AppLog::AppLog() 
	: m_level(LOG_LEVEL_INFO)
	, m_max_size(DEFAULT_LOG_MAX_SIZE)
	, m_max_num(DEFAULT_LOG_MAX_NUM)
	, m_file(NULL)
	, m_is_running(false)
	, m_file_path("")
	, m_err_file_path("")
	, m_append_console(true)
{
}

AppLog::~AppLog() 
{
	if (NULL != m_file) {
		fprintf(m_file, "[%s]  Exit\n", GetNowTime());
		fprintf(m_file, "--------------------------------  Log Tail  --------------------------------\n\n\n");
		fclose(m_file);
		m_file = NULL;
	}
}

bool AppLog::Comsume(const LogData& val) 
{
	if (LOG_LEVEL_OFF != m_level && val.level <= m_level) {
		RollLogFile();
		const char* time_string = GetNowTime();
		const char* level_string = LevelToString(val.level);
		int len = strlen(val.file_name);
		int i;
		for (i = len - 4; i >= 0; --i)
		{
			if (val.file_name[i] == '\\' || val.file_name[i] == '/') { break; }
		}
		const char* file_name = val.file_name + i + 1;

		fprintf(m_file, "[%s]  %8s  %s  (%s:%d)\n", time_string, level_string, val.message, file_name, val.line);
		fflush(m_file);

		if (LOG_LEVEL_ERROR == val.level)
		{
			FILE* fp = fopen(m_err_file_path.c_str(), "a");
			if (fp)
			{
				fprintf(fp, "[%s]  %8s  %s  (%s:%d)\n", time_string, level_string, val.message, file_name, val.line);
				fclose(fp);
			}
		}

		if (m_append_console)  
		{
			cout<<time_string<<"  "<<level_string<<"  "<<val.message<<"  (" << file_name <<":"<<val.line<<")"<<endl;
		}
	}
	return true;
}

AppLog* AppLog::GetInstance() 
{
	if (NULL == m_instance) {
		m_instance = new AppLog();
		m_instance->InitLog();
		m_instance->Start();
	}

	return m_instance;
}

void AppLog::InitLog() 
{
	if (m_is_running) { return; }	//日志线程开始运行后不能再改变值.

	//配置文件中有配置，读取配置；否则用默认值
	std::string conf_path = Global::GetInstance()->GetConfigDir()+"config.xml";
	do 
	{
		if (!QFile::exists(conf_path.c_str())) { break; }
		XmlConfig config(conf_path);
		if (!config.Load()) { break; }

		int max_size = atoi(config.GetValue("Log/MaxSize").c_str());
		int max_num = atoi(config.GetValue("Log/MaxNum").c_str());
		string level_str = config.GetValue("Log/Level");
		m_append_console = atoi(config.GetValue("Log/AppendComsole").c_str());

		set_max_size(max_size);
		set_max_num(max_num);
		set_level(StringToLevel(level_str.c_str()));
	} while (0);

	// 设置log文件的名字.
	std::string app_name = Global::GetInstance()->GetAppName();
	if (app_name == "")
	{
		if (QCoreApplication::instance() != NULL)
		{
			app_name = QCoreApplication::applicationName().toStdString();
		}
		else
		{
			app_name = "Log";
		}
	}
	
	m_file_path = Global::GetInstance()->zhongan_home + "/log/" + app_name + ".log";
	m_err_file_path = Global::GetInstance()->zhongan_home + "/log/" + app_name + "_Error.log";
	OpenLogFile();
	m_is_running = true;
}

LogLevel AppLog::StringToLevel(const char* value) {
	if (NULL == value){ return m_level; }

	LogLevel level = m_level;
	if ((strcmp(value,"LEVEL_OFF") == 0)     || (strcmp(value,"level_off") == 0))     { level = LOG_LEVEL_OFF; }
	if ((strcmp(value,"LEVEL_ERROR") == 0)   || (strcmp(value,"level_error") == 0))   { level = LOG_LEVEL_ERROR; }
	if ((strcmp(value,"LEVEL_WARN") == 0)    || (strcmp(value,"level_warm") == 0))    { level = LOG_LEVEL_WARN; }
	if ((strcmp(value,"LEVEL_INFO") ==0 )    || (strcmp(value,"level_info") == 0))    { level = LOG_LEVEL_INFO; }
	if ((strcmp(value,"LEVEL_SUCCESS") == 0) || (strcmp(value,"level_success") == 0)) { level = LOG_LEVEL_SUCCESS; }
	if ((strcmp(value,"LEVEL_DEBUG") == 0)   || (strcmp(value,"level_debug") == 0))   { level = LOG_LEVEL_DEBUG; }

	return level;
}

const char*  AppLog::LevelToString(LogLevel level) {
	switch (level) {
	case LOG_LEVEL_OFF:
		return "[OFF]";
	case LOG_LEVEL_ERROR:
		return "[ERROR]";
	case LOG_LEVEL_WARN:
		return "[WARM]";
	case LOG_LEVEL_INFO:
		return "[INFO]";
	case LOG_LEVEL_SUCCESS:
		return "[SUCCESS]";
	case LOG_LEVEL_DEBUG:
		return "[DEBUG]";
	default:
		return "";
	}
}

const char* AppLog::GetNowTime() 
{
	static char date_str[25];
#ifdef WIN32
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf(date_str,"%4d-%02d-%02d %02d:%02d:%02d.%03d", sys.wYear,sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
#else
	struct timeval t_time;
	gettimeofday(&t_time, NULL);
	time_t t_date;
	time(&t_date);
	tm* local_t = localtime(&t_date);
	sprintf(date_str,"%4d-%02d-%02d %02d:%02d:%02d.%03ld", local_t->tm_year+1900, local_t->tm_mon+1, local_t->tm_mday, local_t->tm_hour, local_t->tm_min, local_t->tm_sec, t_time.tv_usec/1000);
#endif
	return date_str;   
}

void AppLog::OpenLogFile() {
	if (m_file_path.empty()) {
		cout<<"the path of app log file is null"<<endl;
#ifdef WIN32
		system("pause");
#endif
		exit(1);
	}

	while(m_file_path.find('\\')!=string::npos) {
		m_file_path.replace(m_file_path.find('\\'),1,"/");
	}
	std::string file_dir = m_file_path.substr(0, m_file_path.rfind("/") + 1);
	if (!Directory::IsDirExist(file_dir)) { Directory::MakeDir(file_dir); }

	m_file = fopen(m_file_path.c_str(), "a");
	if (NULL == m_file) {
		//fprintf(stderr, "can't open the log file : %s\n", m_file_path.c_str());
		cout<<"can't open the log file : "<<m_file_path<<endl;
#ifdef WIN32
		system("pause");
#endif
		exit(1);
	}
	fprintf(m_file, "--------------------------------  Log Head  --------------------------------\n");
}

void AppLog::RollLogFile() {
	if (NULL == m_file || "" == m_file_path) { return; }

	if (ftell(m_file) > m_max_size) {
		char origFile[1024];
		char destFile[1024];
		fprintf(m_file, "--------------------------------  Log Tail  --------------------------------\n\n\n");
		fclose(m_file);

		if (1 == m_max_num) {
			remove(m_file_path.c_str());
			OpenLogFile();
			return;
		}

		std::string::size_type pos = m_file_path.find_last_of(".");
		std::string prefix_str = m_file_path.substr(0, pos);
		std::string postfix_str = m_file_path.substr(pos);
		sprintf_s(destFile, 1024, "%s%d%s", prefix_str.c_str(), m_max_num-1, postfix_str.c_str());
		remove(destFile);	//即使文件不存在也不会出现问题.

		for (int i=(m_max_num-1); i > 0; --i) {
			/*add number to old files*/
			if (i > 1) {
				sprintf_s(origFile, 1024, "%s%d%s", prefix_str.c_str(), i-1, postfix_str.c_str());
			}
			else {
				sprintf_s(origFile, 1024, "%s", m_file_path.c_str());
			}
			sprintf_s (destFile, 1024, "%s%d%s", prefix_str.c_str(), i, postfix_str.c_str());

			rename (origFile, destFile);	//即使origFile不存在也没问题.
		}

		OpenLogFile();
	}
}


}
}