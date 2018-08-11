#include "MiniAprPch.h"
#include "ProfilerTimer.h"
#include "util/tc_timeprovider.h"

using namespace MINIAPR;

extern "C"  IComponent* createProfileTimer(Int32 /*dwVersion*/)
{
	return new ProfilerTimer;
}

extern inline unsigned long long __attribute__((always_inline)) rdtsc64() {
    unsigned int hi, lo;
        __asm__ __volatile__(
            /*"xorl %%eax, %%eax\n\t"
            "cpuid\n\t"*/
            "rdtsc"
        : "=a"(lo), "=d"(hi)
        : /* no inputs */
        : "rbx", "rcx");
	return ((unsigned long long)hi << 32ull) | (unsigned long long)lo;
}


ProfilerTimer::ProfilerTimer()
{
	resetTimer();
}

ProfilerTimer::~ProfilerTimer()
{
}

Uint64 ProfilerTimer::getMicroseconds()
{
	if(m_dCpuMZ == 0.0)
	{
		m_dCpuMZ = TC_TimeProvider::getInstance()->cpuMHz();
	}

	Uint64 ddResult = rdtsc64();//m_timerHelp.getMicroSecond();
	ddResult = (Uint64) ( (double)ddResult/m_dCpuMZ);
	
	return ddResult;
}

void ProfilerTimer::resetTimer()
{
	m_timerHelp = TimerHelp();
	m_dCpuMZ = 0.0;
}

