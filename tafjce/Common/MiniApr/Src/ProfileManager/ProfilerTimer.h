#ifndef __PROFILER_TIMER_H__
#define __PROFILER_TIMER_H__

#include "MiniAprType.h"
#include "TimerManager/TimerManager.h"

BEGIN_MINIAPR_NAMESPACE

class ProfilerTimer:public ComponentBase<IProfilerTimer, IID_IProfilerTimer>
{
public:

	ProfilerTimer();
	~ProfilerTimer();

	virtual Uint64 getMicroseconds();
	virtual void resetTimer();

private:

	TimerHelp	m_timerHelp;
	double m_dCpuMZ;
};

END_MINIAPR_NAMESPACE

#endif
