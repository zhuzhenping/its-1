/*!
* \brief       互斥锁.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
* Mutex mutex;
* mutex.Lock();
* //需要保护的操作.
* mutex.Unlock();
*
*/

#ifndef _COMMON_MUTEX_H_  
#define _COMMON_MUTEX_H_  

#include <assert.h> 
#if defined WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>  
#else  
#include <pthread.h>  
#include <errno.h>
#endif  

#include "common/Global.h"
#include "common/Locker.h"

namespace zhongan {
namespace common {

//不允许重复锁.
class COMMON_API Mutex : public LockBase
{
#if defined WIN32
    mutable CRITICAL_SECTION m_mutex;
#else
    mutable pthread_mutex_t m_mutex;
#endif

public :
    Mutex ();

    ~Mutex ();

#if defined WIN32
	inline CRITICAL_SECTION* InnerMutex() { return &m_mutex; }
#else
	inline pthread_mutex_t* InnerMutex() { return &m_mutex; }
#endif

    /**
     * lock the mutex, which may blocked the thread
     */
    inline void Lock();

    /**
     * unlock the mutex
     */
    inline void Unlock();

    /**
     * try lock the mutex
     * @return
     *      true, if lock successed
     */
    inline bool Trylock();
};

inline void Mutex::Lock()
{
#if defined WIN32
	EnterCriticalSection(&m_mutex);  
	assert(m_mutex.RecursionCount == 1);
#else
    int rs = ::pthread_mutex_lock(&m_mutex);
    assert(0 == rs);
#endif
}

inline void Mutex::Unlock()
{
#if defined WIN32
	assert(m_mutex.RecursionCount == 1);
	LeaveCriticalSection(&m_mutex);
#else
    int rs = ::pthread_mutex_unlock(&m_mutex);
    assert(0 == rs);
#endif
}

inline bool Mutex::Trylock()
{
#if defined WIN32
	if(!TryEnterCriticalSection(&m_mutex)) { return false; }

	if(m_mutex.RecursionCount > 1) {
		LeaveCriticalSection(&m_mutex);
		throw ("Thread locked expcetion! LeaveCriticalSection");
	}
	return true;
#else
    int rc = ::pthread_mutex_trylock(&m_mutex);
	if(rc != 0 && rc != EBUSY) {
		throw ("Invalid mutex");
	}
    /** returned values :
        *  0, lock ok
        *  EBUSY, The mutex is already locked.
        *  EINVAL, Mutex is not an initialized mutex.
        *  EFAULT, Mutex is an invalid pointer.
        */
    return 0 == rc;
#endif
}

/*
class MutexLocker {
public:
	MutexLocker(Mutex* mutex) : m_mutex(mutex) {
		m_mutex->Lock();
	}

	~MutexLocker() {
		m_mutex->Unlock();
	}

private:
	Mutex* m_mutex;
};
*/
}
}

#endif