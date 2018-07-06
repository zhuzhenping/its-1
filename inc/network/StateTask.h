#ifndef TIS_STRATEGY_STATE_TASK_H_  
#define TIS_STRATEGY_STATE_TASK_H_  

#include <map>
#include "common/Global.h"
#include "common/ExceptionErr.h"

//namespace itstation {

class StateTaskManager;

class StateTask
{
public:
	StateTask(int task_id) : task_id_(task_id), success_(true) {}
	int GetId() { return task_id_; } // 获取状态.

	virtual bool Handle(StateTaskManager* manager) = 0;
	virtual int ToNextState() = 0; //如果没跳到下个任务，则返回-1.

	bool success_;

protected:
	int task_id_; //非负数  状态信息.
};

class StateTaskManager
{
public:
	StateTaskManager();
	virtual ~StateTaskManager();

 	void InsertTask(StateTask* task);
	void SetCurTask(int id);
	void HandleNextTask();

protected:
	std::map<int, StateTask*> tasks_;
	StateTask* cur_task_;
};


#endif