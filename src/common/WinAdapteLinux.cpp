#include "common/WinAdapteLinux.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <string.h>
#include <iconv.h>
#endif

#ifdef WIN32
long long atoll (const char *p)  
{  
	while (*p == ' ')
		p++;
	int minus = 0;  
	long long value = 0;  
	if (*p == '-')  
	{  
		minus ++;  
		p ++;  
	}  
	while (*p >= '0' && *p <= '9')  
	{  
		value *= 10;  
		value += *p - '0';  
		p ++;  
	}  
	return minus ? 0 - value : value;  
} 

std::string UTF8ToMB(const std::string& str)  
{  
	int utf8Len = str.length();
	int nLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), utf8Len, NULL, 0);  

	WCHAR* lpszW = NULL;  
	try  
	{  
		lpszW = new WCHAR[nLen];  
	}  
	catch(std::bad_alloc &)  
	{  
		return "";  
	}  

	int nRtn = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), utf8Len, lpszW, nLen);  

	if(nRtn != nLen)  
	{  
		delete[] lpszW;  
		return "";  
	}  

	// convert an widechar string to Multibyte   
	int MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);  
	if (MBLen <=0)  
	{  
		return "";  
	}  
	char* dest = new char[MBLen + 1];
	memset(dest, 0, MBLen + 1);
	nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, dest, MBLen, NULL, NULL);  
	delete[] lpszW;  
	std::string result = dest;  
	delete [] dest;

	if(nRtn != MBLen)  
	{  
		return "";  
	}  
	return result;  
}

std::string MBToUTF8(const std::string& str)
{
	// convert an MBCS string to widechar 
	int len = str.length();
	int nLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), len, NULL, 0);

	WCHAR* lpszW = NULL;
	try
	{
		lpszW = new WCHAR[nLen];
	}
	catch(std::bad_alloc &memExp)
	{
		return "";
	}

	int nRtn = MultiByteToWideChar(CP_ACP, 0, str.c_str(), len, lpszW, nLen);

	if(nRtn != nLen)
	{
		delete[] lpszW;
		return "";
	}
	// convert an widechar string to utf8
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, NULL, 0, NULL, NULL);
	if (utf8Len <= 0)
	{
		return false;
	}

	char* dest = new char[utf8Len + 1];
	memset(dest, 0, utf8Len + 1);
	nRtn = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, dest, utf8Len, NULL, NULL);
	delete[] lpszW;
	std::string result = dest;  
	delete [] dest;

	if (nRtn != utf8Len)
	{
		return "";
	}

	return result;   
}

#else
int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
	iconv_close(cd);
	return 0;
}

std::string UTF8ToMB(const std::string &sourcebuf) {
	char out[50];
	int ret = code_convert("utf-8","gb2312", (char*)sourcebuf.c_str(), sourcebuf.size(), out, sizeof(out));
	if (ret == 0)
		return out;
	else
		return "";
}

std::string MBToUTF8(const std::string& sourcebuf) {
	char out[50];
	int ret = code_convert("gb2312", "utf-8", (char*)sourcebuf.c_str(), sourcebuf.size(), out, sizeof(out));
	if (ret == 0)
		return out;
	else
		return "";
}

#endif

