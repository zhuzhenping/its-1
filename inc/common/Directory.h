#ifndef EYEGLE_COMMON_DIRECTORY_H_
#define EYEGLE_COMMON_DIRECTORY_H_

#include <string>
#include <set>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <tchar.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include "common/Global.h"

#ifdef WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

bool COMMON_API IsDirExist(const std::string& dir_path);

bool COMMON_API MakeDir(const std::string& dirpath);

void COMMON_API GetFilesInDir(std::string path, std::set<std::string>& files);
void COMMON_API GetDirsInDir(std::string path, std::set<std::string>& dirs);

// 设置当前路径的上两个文件夹路径为ITS_HOME环境变量;如果环境变量已设，直接返回
bool COMMON_API SetItsHome();

// 获取当前全路径
std::string COMMON_API GetCurrentPath();

//获取和设置相对路径中的当前路径
std::string COMMON_API GetRelativeCurrent();
bool COMMON_API SetRelativeCurrent(const std::string& path);

#endif