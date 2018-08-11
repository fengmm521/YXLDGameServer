/*********************************************************
*
*	名称: IprofilerTimer.h
*	作者: wuxf
*	时间: 2011-04-18
*	描述: 用于性能监视器计时处理
*********************************************************/

#ifndef __IPROFILER_TIMER_H__
#define __IPROFILER_TIMER_H__

#define IID_IProfilerTimer	MAKE_RID('p','r','t','m')

BEGIN_MINIAPR_NAMESPACE

class IProfilerTimer:public IComponent
{
public:
	
	/*!
	\brief 
		获取自上次重置以来的微妙时间
	\return Uint32 
	*/
	virtual Uint64 getMicroseconds() = 0;
	

	/*!
	\brief 
		重置定时器，从这之后开始计时
	\return void 
	*/
	virtual void resetTimer() = 0;
};

END_MINIAPR_NAMESPACE

#endif
