#include "datalib/StateTask.h"
#include "common/DateTime.h"

//namespace itstation {

StateTaskManager::StateTaskManager() : cur_task_(NULL)
{

}

StateTaskManager::~StateTaskManager()
{
	for (std::map<int, StateTask*>::iterator iter = tasks_.begin(); iter != tasks_.end(); ++iter)
	{
		StateTask* task = iter->second;
		delete task;
	}
}

void StateTaskManager::InsertTask(StateTask* task)
{
	if (task->GetId() < 0) { ThrowError("invalid task id"); }
	if (tasks_.find(task->GetId()) != tasks_.end()) { ThrowError("duplicate task id"); }

	tasks_[task->GetId()] = task;
}

void StateTaskManager::SetCurTask(int id)
{
	std::map<int, StateTask*>::iterator iter = tasks_.find(id);
	if (iter == tasks_.end())
	{
		ThrowError("task is not exist: " << id);
	}

	cur_task_ = iter->second;
}

void StateTaskManager::HandleNextTask()
{
	if (cur_task_ == NULL) { ThrowError("current task is null"); }

	if (!cur_task_->success_)
	{
		DateTime now(NULL);
		if (now.IsTradingDay() && now.date.WeekDay()!=6)
			cur_task_->success_ = cur_task_->Handle(this);
		else
			cur_task_->success_ = true;
	} 
	else 
	{
		int next_task_id = -1;
		if ((next_task_id = cur_task_->ToNextState()) >= 0)//如果状态发生变化了.
		{
			std::map<int, StateTask*>::iterator iter = tasks_.find(next_task_id);
			if (iter == tasks_.end())
			{
				ThrowError("next task is not exist: " << next_task_id);
			}

			cur_task_ = iter->second;
			cur_task_->success_ = cur_task_->Handle(this);
		}
	}
}

