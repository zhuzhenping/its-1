#include <stdlib.h>
#include <iostream>
#include "common/Global.h"


Global* Global::m_instance = NULL;

Global::Global() : m_app_conf_file_name("config.xml")
{
}

Global* Global::GetInstance() {
	if (NULL == m_instance) {
		m_instance = new Global();
		m_instance->Init();
	}

	return m_instance; 
}

void Global::Init() {
	/*char* its_home_c = getenv("ITS_HOME");
	if(NULL == its_home_c) { 
		throw std::runtime_error("ITS_HOME环境变量未设置");
	}*/
#ifdef WIN32
	its_home = "C:/Users/za-wudian/Desktop/its";
#else
	its_home = "/home/wd/its";
#endif
}

std::string Global::GetConfigDir() const
{
	return its_home + "/cfg/";
}

void Global::SetAppConfigFileName(const std::string& file_name)
{
	m_app_conf_file_name = file_name;
}

std::string Global::GetAppConfigPath() const
{
	return GetConfigDir() + m_app_conf_file_name;
}

void Global::SetAppName(const std::string& name)
{
	m_app_name = name;
}

std::string Global::GetAppName() const
{
	return m_app_name;
}
