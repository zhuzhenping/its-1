#ifndef READ_WRITE_DATA_FILE_H
#define READ_WRITE_DATA_FILE_H

#include <string>
#include <deque>
#include <QtCore/QFile>
#include "common/Global.h"
#include "datalib/DataServerStruct.h"
#include "datalib/Protocol.h"
#include "common/ExceptionErr.h"
#include "common/AppLog.h"



template<class T>
QFile* MapFile(const std::string& path, T** data, int* num)
{
	if (!QFile::exists(path.c_str())) { return NULL; }
	QFile* file = new QFile(path.c_str());
	if (!file->open(QIODevice::ReadOnly))
	{
		APP_LOG(LOG_LEVEL_ERROR) << "open file error: " << path;
		delete file;
		return NULL;
	}
	if (file->size() == 0) {
		*num = 0;
		return file;
	}
	if ((file->size() % sizeof(T) != 0) || ((*data = (T*)file->map(0, file->size())) == NULL))
	{
		APP_LOG(LOG_LEVEL_ERROR) << "file size or map error: " << path;
		file->close();
		delete file;
		return NULL;
	}

	*num = file->size() / sizeof(T);
	return file;
}

//从文件取特定根数的数据存入deque ; num==-1表示获取所有数据.
template<class T> 
void ReadDatasWithNum(std::deque<T> &deq_datas, const std::string& path, int num = -1)
{
	int len; T* data;
	QFile* file = MapFile(path, &data, &len);
	if (file == NULL) { return; }

	if (-1 == num) num = len;
	for (int i=len-1; i>=0; --i)
	{
		if (num <= 0) { break; }
		--num;
		deq_datas.push_front(data[i]);
	}

	file->unmap((uchar*)data);
	file->close();
	delete file;
}


template<typename T>
const SimpleDateTime& GetDateTime(const T& data)
{
	return data.date_time;
}

template<>
const SimpleDateTime& GetDateTime<FutureKline>(const FutureKline& data)
{
	return data.b_time;
}

//将数据写到文件首或文件尾.
template<class T>
void WriteDatas(const std::deque<T> &deq_datas, const std::string& path) {
	// 如果文件不存在 则创建文件并写入所有数据.
	if (!QFile::exists(path.c_str())) {
		FILE *fp = fopen(path.c_str(), "ab+");
		if (fp == NULL) {
			APP_LOG(LOG_LEVEL_ERROR) << "open file error: " << path;
			return;
		}				
		for (typename std::deque<T>::const_iterator iter = deq_datas.begin(); iter != deq_datas.end(); ++iter)
			fwrite(&(*iter), sizeof(T), 1, fp);		
		fclose(fp);
		return;
	}

	if (deq_datas.empty()) return;

	int len; T* data;
	QFile* file = MapFile(path, &data, &len);
	if (file == NULL) { return; }

	if (0 == len) {
		file->close();
		delete file;
		FILE *fp = fopen(path.c_str(), "ab+");
		if (fp == NULL) { APP_LOG(LOG_LEVEL_ERROR) << "open file error: " << path; return; }
		for (typename std::deque<T>::const_iterator iter = deq_datas.begin(); iter != deq_datas.end(); ++iter)
			fwrite(&(*iter), sizeof(T), 1, fp);
		fclose(fp);
	}
	else if (GetDateTime(deq_datas.back()) <= GetDateTime(*data)) {
		file->unmap((uchar*)data);
		file->close();
		delete file;

		std::deque<T> end_datas;
		ReadDatasWithNum(end_datas, path);

		FILE *fp = fopen(path.c_str(), "wb+");
		if (fp == NULL) { APP_LOG(LOG_LEVEL_ERROR) << "open file error: " << path; return; }
		for (typename std::deque<T>::const_iterator iter = deq_datas.begin(); iter != deq_datas.end(); ++iter)
			fwrite(&(*iter), sizeof(T), 1, fp);
		for (typename std::deque<T>::const_iterator iter = end_datas.begin(); iter != end_datas.end(); ++iter)
			fwrite(&(*iter), sizeof(T), 1, fp);
		fclose(fp);
	}
	else if (GetDateTime(deq_datas.front()) >= GetDateTime(*(data + (len-1))) ) {
		file->unmap((uchar*)data);
		file->close();
		delete file;

		FILE *fp = fopen(path.c_str(), "ab+");
		if (fp == NULL) { APP_LOG(LOG_LEVEL_ERROR) << "open file error: " << path; return; }
		for (typename std::deque<T>::const_iterator iter = deq_datas.begin(); iter != deq_datas.end(); ++iter)
			fwrite(&(*iter), sizeof(T), 1, fp);
		fclose(fp);
	}
	else {
		APP_LOG(LOG_LEVEL_ERROR) << "写入数据的范围有误.";
		file->close();
		return;
	}
}




#endif