/*!
* \brief       简化加锁、解锁的过程.
* \author      吴典@众安科技虚拟实验室.
* \date        -
*
* \usage
* SpinLock spin_lock;
* Locker locker(&spin_lock);
* //需要保护的操作.
* 
*
* \usage
* Mutex mutex;
* Locker locker(&mutex);
* //需要保护的操作.
*
*/

#ifndef _COMMON_LOCKER_H_  
#define _COMMON_LOCKER_H_  


namespace zhongan {
namespace common {

class LockBase
{
public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
	virtual bool Trylock() = 0;
};

class Locker {
public:
	Locker(LockBase* lock) : m_lock(lock) {
		m_lock->Lock();
	}

	~Locker() {
		m_lock->Unlock();
	}

private:
	LockBase* m_lock;
};

}
}

#endif