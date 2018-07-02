#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include "common/Global.h"
#include "common/Directory.h"
#include "common/Mutex.h"

Global* Global::m_instance = NULL;

Global::Global() 
{
}

Global* Global::GetInstance() {
	static Mutex s_mutex;
	Locker lock(&s_mutex);
	if (NULL == m_instance) {
		m_instance = new Global();
		m_instance->Init();
	}

	return m_instance; 
}

void Global::Init() {
	char* its_home_c = getenv("ITS_HOME");
	if (NULL == its_home_c) {
		Directory::SetItsHome();
	}
	its_home_c = getenv("ITS_HOME");
	if(NULL == its_home_c) { 
		throw std::runtime_error("ITS_HOME环境变量未设置");
	}
	its_home = its_home_c;


	m_app_name = Directory::GetAppName();
}

std::string Global::GetConfigDir() const
{
	return its_home + "/cfg/";
}

std::string Global::GetPluginsDir() const
{
	return its_home + "/bin/plugins/";
}

std::string Global::GetAppName() const
{
	return m_app_name;
}
