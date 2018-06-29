/*!
* \brief       环境变量、目录、文件的基本操作.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
*
*
*/

#ifndef _COMMON_DIRECTORY_H_
#define _COMMON_DIRECTORY_H_

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

namespace zhongan {

class COMMON_API Directory
{
public:
	// 判断某个目录是否存在.
	static bool IsDirExist(const std::string& dir_path);
	// 创建某个目录.
	static bool MakeDir(const std::string& dirpath);

	// 取目录下所有文件.
	static void GetFilesInDir(std::string path, std::set<std::string>& files);
	// 取目录下所有文件夹名.
	static void GetDirsInDir(std::string path, std::set<std::string>& dirs);


	// 取绝对路径（默认取当前路径）.
	static std::string GetCurrentPath(std::string = "");
	// 获取应用程序的全路径. ../../app.exe
	static std::string GetAppPath();

	
	/*// 设置相对路径（相对当前目录）.
	static bool SetRelativeCurrent(const std::string& path);
	// 取 设完相对路径后 的当前路径.
	static std::string GetRelativeCurrent();*/


private:
	friend class Global;
	//
	static bool SetZhonganHome();
};

}

#endif