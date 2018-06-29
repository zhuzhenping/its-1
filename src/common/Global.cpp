#include <stdlib.h>
#include <iostream>
#include "common/Global.h"
#include "common/Directory.h"

using namespace std;

namespace zhongan {

Global* Global::m_instance = NULL;

Global::Global() 
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
	char* zhongan_home_c = getenv("ZHONGAN_HOME");
	if (NULL == zhongan_home_c) {
		Directory::SetZhonganHome();
	}
	zhongan_home_c = getenv("ZHONGAN_HOME");
	if(NULL == zhongan_home_c) { 
		throw std::runtime_error("ZHONGAN_HOME环境变量未设置");
	}
	zhongan_home = zhongan_home_c;


	string app_path = Directory::GetAppPath();
	size_t _bgn = app_path.find_last_of("\\") + 1;
	size_t _end = app_path.find_last_of(".");
	m_app_name = app_path.substr(_bgn, _end - _bgn);
}

std::string Global::GetConfigDir() const
{
	return zhongan_home + "/cfg/";
}

std::string Global::GetPluginsDir() const
{
	return zhongan_home + "/bin/plugins/";
}

std::string Global::GetAppName() const
{
	return m_app_name;
}

}