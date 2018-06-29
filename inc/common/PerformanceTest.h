#ifndef COMMON_PERFORMANCE_TEST_H_
#define COMMON_PERFORMANCE_TEST_H_

#include "common/SpendTime.h"
#include "common/Global.h"
#include <vector>
#include <iostream>

enum TestIndex{
	DataStroerObj_PushTick
};
class COMMON_API PerformanceTest
{
	enum { MAX_TEST_NUM = 32 };
	struct PerformanceInfo
	{
		int call_num;
		double sum_cost;
		SpendTime timer;

		PerformanceInfo() : call_num(0), sum_cost(0) {}
	};
public:
	void Init()
	{
		static const char* test_names[] = {
			"DataStroerObj_PushTick",
			NULL
		};
		Init(test_names);
	}

	void Init(const char* test_names[])
	{
		if (names_.size() > 0) { return; } //已经初始过

		for (int i=0; i < MAX_TEST_NUM; ++i)
		{
			if (NULL == test_names[i]) { break; }
			names_.push_back(test_names[i]);
		}

		if (names_.size() > 0) { infos_ = new PerformanceInfo[names_.size()]; }
	}

	void StartTimer(int i)
	{
		infos_[i].timer.Start();
	}
	void StopTimer(int i)
	{
		infos_[i].timer.Stop();
		infos_[i].call_num++;
		infos_[i].sum_cost += infos_[i].timer.Seconds();
	}
	void Clear()
	{
		for (int i = 0; i < names_.size(); ++i)
		{
			infos_[i].call_num = 0;
			infos_[i].sum_cost = 0;
		}
	}
	void Print(int idx = -1)
	{
		if (idx == -1)
		{
			for (int i = 0; i < names_.size(); ++i)
			{
				if (infos_[i].call_num == 0) { continue; }
				Print(i);
			}
			return;
		}

		std::cout << idx << "] " << names_[idx].c_str() << " :  num=" << infos_[idx].call_num
			<< "  sum=" << infos_[idx].sum_cost << "  avg=" 
			<< infos_[idx].sum_cost / infos_[idx].call_num << std::endl;
	}

public:
	static PerformanceTest* inst_;

private:
	PerformanceInfo* infos_;
	std::vector<std::string> names_;
};

#ifdef PERFORMANCE_TEST
#define PTEST_INIT() PerformanceTest::inst_->Init()
#define PTEST_CLEAR() PerformanceTest::inst_->Clear()
#define PTEST_START(i) PerformanceTest::inst_->StartTimer(i)
#define PTEST_STOP(i) PerformanceTest::inst_->StopTimer(i)
#define PTEST_PRINT(i) PerformanceTest::inst_->Print(i)
#else
#define PTEST_INIT()
#define PTEST_CLEAR()
#define PTEST_START(i)
#define PTEST_STOP(i)
#define PTEST_PRINT(i)
#endif

#endif // THERON_BENCHMARKS_COMMON_TIMER_H

