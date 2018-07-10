#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include "common/Directory.h"
#include "common/AppLog.h"


bool Directory::IsDirExist(const std::string& dir_path)
{
	return ACCESS(dir_path.c_str(),0) != -1;
}

bool Directory::MakeDir(const std::string& dirpath)
{	
	std::string pathtemp;
	pathtemp = dirpath;
	std::string tempdir;
	while(pathtemp.find('\\') != std::string::npos) {
		pathtemp.replace(pathtemp.find('\\'),1,"/");
	}

	if(pathtemp[pathtemp.length()-1]!='/') {
		pathtemp+="/";
	}

	while(pathtemp.find('/') != std::string::npos)
	{
		tempdir.insert(tempdir.length(),pathtemp,0,pathtemp.find('/')+1);
		pathtemp=pathtemp.substr(pathtemp.find('/')+1);
		int result=0;
		if(!IsDirExist(tempdir) && (0 != MKDIR(tempdir.c_str()))) { return false; }
	}

	return true;
}

//void GetFilesInDir(std::string path, std::set<std::string>& files) 
//{
//	while(path.find('\\') != std::string::npos) 
//	{
//		path.replace(path.find('\\'),1,"/");
//	}
//	if (!IsDirExist(path)) { return; }
//
//#ifdef WIN32
//	_finddata_t info;
//	path += path[path.length()-1] != '/' ? "/*" : "*";
//	long handle = _findfirst(path.c_str(), &info);
//	if (handle == -1L) { return; }
//
//	do{
//		if (!(info.attrib & _A_SUBDIR)) { files.insert(info.name); }   
//	}while (_findnext(handle, &info) == 0);
//
//	_findclose(handle);
//#else
//	DIR              *pDir ;  
//	struct dirent    *ent  ;  
//	int               i=0  ;  
//
//	pDir = opendir(path.c_str());  
//
//	while((ent = readdir(pDir)) != NULL)  
//	{  
//		if(!(ent->d_type & DT_DIR)) { files.insert(ent->d_name); }
//	} 
//#endif
//}

void Directory::GetFilesInDir(std::string path, std::set<std::string>& files)
{
	QTextCodec *codec = QTextCodec::codecForName("GB2312");
	QDir dir(path.c_str());	
	QFileInfoList infos = dir.entryInfoList(QDir::Files);
	for (int i=0; i < infos.size(); ++i)
	{
		files.insert(NULL == codec ? infos[i].fileName().toLocal8Bit().constData()
			: codec->fromUnicode(infos[i].fileName()).data());
	}
}

void Directory::GetDirsInDir(std::string path, std::set<std::string>& dirs)
{
	QTextCodec *codec = QTextCodec::codecForName("GB2312");
	QDir dir(path.c_str());	
	QFileInfoList infos = dir.entryInfoList(QDir::Dirs | QDir::NoDot | QDir::NoDotDot);
	for (int i=0; i < infos.size(); ++i)
	{
		dirs.insert(NULL == codec ? infos[i].fileName().toLocal8Bit().constData()
			: codec->fromUnicode(infos[i].fileName()).data());
	}
}

bool Directory::SetItsHome()
{
	//已设置，直接返回
	char* its_home = getenv("ITS_HOME");
	if (NULL != its_home) { return true; }

	/*QDir dir = QDir::current();
	if (!dir.exists()) { return false; }

	std::string value = GetCurrentPath();
	if(!dir.exists("config")){ // bin/Debug
		if (!dir.cdUp()) { return false; }
		if (!dir.cdUp()) { return false; }	
		value = QDir::toNativeSeparators(dir.path()).toLocal8Bit().constData()();
	}*/
	string app_path = GetAppPath();
	size_t _bin = app_path.find("bin");
	string value = GetCurrentPath(app_path.substr(0, _bin));
#ifdef WIN32
	if (putenv(QObject::tr("ITS_HOME=%1").arg(value.c_str()).toLocal8Bit().constData())) return false;
#else
	if (setenv("ITS_HOME", (char*)value.c_str(), 1)) return false;
#endif
	return true;
}

std::string Directory::GetCurrentPath(string path) {
	QDir dir("" == path? "./" : path.c_str());
	return dir.absolutePath().toLocal8Bit().constData();
}

std::string Directory::GetAppPath(){
#ifdef WIN32
	char szFullPath[MAX_PATH];
	ZeroMemory(szFullPath, MAX_PATH);
	GetModuleFileNameA(NULL, szFullPath, MAX_PATH);
	return szFullPath;
#else
	const int MAXBUFSIZE = 1024;
	char buf[ MAXBUFSIZE ];
	memset(buf, 0 , MAXBUFSIZE);
	//getcwd(buf, MAXBUFSIZE);
	int cnt = readlink("/proc/self/exe", buf, MAXBUFSIZE );
	return buf;
#endif
}

std::string Directory::GetAppName(){
	string app_path = Directory::GetAppPath();
#ifdef WIN32
	size_t _bgn = app_path.find_last_of("\\") + 1;
	size_t _end = app_path.find_last_of(".");
	return app_path.substr(_bgn, _end - _bgn);
#else
	size_t _bgn = app_path.find_last_of("/") + 1;
	size_t _end = app_path.size();
	return app_path.substr(_bgn, _end - _bgn);
#endif
}

/*
std::string Directory::GetRelativeCurrent()
{
	QDir dir = QDir::current();
	if (!dir.exists()) { return ""; }
	return QDir::toNativeSeparators(dir.path()).toLocal8Bit().constData()();
}

bool Directory::SetRelativeCurrent(const std::string& path)
{
	return QDir::setCurrent(path.c_str());
}*/

