#ifndef _COMMON_QUEUE_BUFFER_H_  
#define _COMMON_QUEUE_BUFFER_H_  

#include <assert.h>
#include "common/Thread.h" 
#include "common/SpinLock.h"
#include "common/Mutex.h"
#include "common/Condition.h"

namespace zhongan {
namespace common {

template <class T, unsigned int LEN>
class QueueBuffer : public Thread {
public:
	QueueBuffer() 
		: head_idx(0)
		, tail_idx(0)
		, m_empty_size(LEN)
		, m_full_size(0)
	{
		assert(LEN > 0);
	}

	virtual bool Comsume(const T& val) = 0;

	inline void Push(const T& val) {
		while (get_empty_size() == 0)
		{
			m_empty_mutex.Lock();
			m_empty_cond.Wait(&m_empty_mutex);
			m_empty_mutex.Unlock();
		}

		m_buf[head_idx] = val;
		idx_lock.Lock();
		head_idx = (head_idx + 1) % LEN;
		m_empty_size--;
		m_full_size++;
		idx_lock.Unlock();
		m_full_cond.Signal();
	}

	void Terminate()
	{
		Stop();
		cond_.Signal();
	}

private:
	unsigned int get_empty_size() {
		Locker locker(&idx_lock);
		return m_empty_size;
	}

	unsigned int get_full_size() {
		Locker locker(&idx_lock);
		return m_full_size;
	}

	void OnRun() {
		while (IsRuning()) {
			while (get_full_size() == 0)
			{
				m_full_mutex.Lock();
				m_full_cond.Wait(&m_full_mutex);
				m_full_mutex.Unlock();
			}

			if (!Comsume(m_buf[tail_idx])) continue;
			idx_lock.Lock();
			tail_idx = (tail_idx + 1) % LEN;
			m_empty_size++;
			m_full_size--;
			idx_lock.Unlock();
			m_empty_cond.Signal();
		}
	}

private:
	T m_buf[LEN];

	Mutex m_empty_mutex;
	Condition m_empty_cond;

	Mutex m_full_mutex;
	Condition m_full_cond;

	mutable int head_idx;
	mutable int tail_idx;

	mutable unsigned int m_empty_size;
	mutable unsigned int m_full_size;

	SpinLock idx_lock;
	SpinLock size_lock;

	common::Condition cond_;
};

}
}

#endif