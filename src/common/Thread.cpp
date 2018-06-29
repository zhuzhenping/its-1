#include "common/Thread.h"

namespace zhongan {
namespace common {

#ifdef WIN32
void SleepImp(int mil_sec)
{
	Sleep(mil_sec);
}
#endif

Thread::Thread() : m_state(STOPED)
{
}

Thread::~Thread()
{
	WaitExit();
}

void Thread::Start() {
	if (GetState() == RUNING) { return; }

	m_state = RUNING;
	//MEM_FUNC mem_func = (MEM_FUNC)&Thread::ThreadProc;
	//FUNC callback = *((FUNC*)&mem_func);  
#ifdef WIN32
	int ret =  _beginthread(ThreadProc, 1000, this);    
	if(ret < 0) {
		throw runtime_error("chreat thread error");
		SetState(STOPED);
		return;
	}

	//HANDLE handle = (HANDLE)ret;
	//bool rett = SetThreadPriority(handle, TransLatePriority());


	//unsigned  uiThread1ID;  
	//HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, callback, this, CREATE_SUSPENDED, &uiThread1ID);
	//cout<<"SetThreadPriority: "<<SetThreadPriority(handle, TransLatePriority())<<endl;
	//cout<<"ThreadPriority: "<<GetThreadPriority(handle)<<endl;
	//SetProcessPriorityBoost(handle, true);
	//ResumeThread( handle );
#else
	pthread_t tid;
	int ret = pthread_create(&tid, 0, ThreadProc, this);
	if(ret) {
		throw runtime_error("chreat thread error");
		SetState(STOPED);
		return;
	}
#endif 
}

void Thread::Join(int sec) {
	if (GetState() == STOPED) { return; }
	wait_mutex_.Lock();
	wait_cond_.TimedWait(&wait_mutex_, sec);
	wait_mutex_.Unlock();
}

void Thread::Stop() {
	if (GetState() == RUNING) { 
		SetState(REQUEST_STOP);
	}
}

void Thread::WaitExit()
{
	if (GetState() == STOPED) { return; }
	wait_mutex_.Lock();
	wait_cond_.TimedWait(&wait_mutex_, 3);
	wait_mutex_.Unlock();
}

void Thread::Awake()
{
	wait_cond_.Signal();
}

int Thread::TransLatePriority() {
#ifdef WIN32
	switch(m_priority) {
	case HIGHEST_PRIORTY:
		return THREAD_PRIORITY_TIME_CRITICAL;
	case ABOVE_PRIORITY:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case BELOW_PRORITY:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case LOWEST_PRIORITY:
		return THREAD_PRIORITY_LOWEST ;
	default:
		return THREAD_PRIORITY_IDLE;
	}
#else

#endif
}

bool Thread::IsRuning() {
	return GetState() == RUNING;
}

void Thread::SetState(ThreadState state) {
	Locker locker(&m_lock);
	m_state = state;
}

Thread::ThreadState Thread::GetState() {
	Locker locker(&m_lock);
	return m_state;
}

}
}