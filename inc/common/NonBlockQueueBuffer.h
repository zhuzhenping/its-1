#ifndef _COMMON_NON_BLOCK_QUEUE_BUFFER_H_  
#define _COMMON_NON_BLOCK_QUEUE_BUFFER_H_  

#include "common/Thread.h" 
#include "common/AppLog.h"
#include <deque>

namespace zhongan {
namespace common {

template <class T>
class NonBlockQueueBuffer : public Thread 
{
public:
	NonBlockQueueBuffer() {}

	virtual void Comsume(T& item) = 0;

	virtual void Push(const T& item)
	{
		common::Locker locker(&queue_lock_);
		eve_queue_.push_back(item);
		cond_.Signal();
	}

	virtual void Terminate()
	{
		Stop();
		cond_.Signal();
	}

private:
	virtual void OnRun() 
	{
		//启动的时候先检查有没有任务
		if (IsRuning())
		{
			T item;
			while (PopEvent(item))
			{
				Comsume(item);
			}
		}

		while (IsRuning()) 
		{
			mutex_.Lock();
			cond_.Wait(&mutex_);
			mutex_.Unlock();
			if (!IsRuning()) { break; }

			T item;
			while (PopEvent(item))
			{
				Comsume(item);
			}
		}

		APP_LOG_DBG << "NonBlockQueueBuffer exit";
	}

	bool PopEvent(T& item)
	{
		common::Locker locker(&queue_lock_);
		if (eve_queue_.size() == 0)
		{
			return false;
		} 
		else
		{
			item = eve_queue_.front();
			eve_queue_.pop_front();
			return true;
		}
	}

protected:
	common::Mutex mutex_;
	common::Condition cond_;
	std::deque<T> eve_queue_;
	SpinLock queue_lock_;
};

}
}

#endif