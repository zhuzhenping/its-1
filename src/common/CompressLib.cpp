#ifdef WIN32

#include "common/CompressLib.h"
#include "common/AppLog.h"


bool ZlibCompress(void * pIn,int nInLen,void * pOut,int * nOutLen)
{
	//int ret=compress((Bytef *)pOut,(uLongf *)nOutLen,(const Bytef *)pIn,nInLen);	
	return compress2((Bytef *)pOut,(uLongf *)nOutLen,(const Bytef *)pIn,nInLen, 9) == Z_OK;
}

bool ZlibUnCompress(void * pIn,int nInLen,void * pOut,int * nOutLen)
{
	int ret = uncompress((Bytef *)pOut,(uLongf *)nOutLen,(const Bytef *)pIn,nInLen);
	if (ret != Z_OK)
	{
		APP_LOG(LOG_LEVEL_ERROR) << "uncompress ERROR:" << ret;
	}
	return ret == Z_OK;
}

int ZlibLimitLen(int len)
{
	return compressBound(len);
}


#endif