/*********************************************************
*
*	名称: IprofilerStrategy.h
*	作者: wuxf
*	时间: 2011-04-18
*	描述: 性能监控策略
*********************************************************/

#ifndef __IPROFILER_STRATEGY_H__
#define __IPROFILER_STRATEGY_H__

#define IID_IProfilerStrategy	 MAKE_RID('p','r','s','r')

BEGIN_MINIAPR_NAMESPACE

class IProfilerStrategy:public IComponent
{
public:

	/*!
	\brief 
		开始计时回调
	\param[in]const string& strProfilerName
		性能监控项名字
	\param[in]Uint64 ddTimer
		计时开始时的时间
	\return void 
	*/
	virtual void onBeginProfiler(const std::string& strProfilerName, Uint64 ddTimer) = 0;

	/*!
	\brief 
		完成计时回调
	\param[in]const string& strProfilerName
		性能监控项名字
	\param[in]Uint64 ddTimer
		完成监控时的时间
	\return void 
	*/
	virtual void onEndProfiler(const std::string& strProfilerName, Uint64 ddTimer) = 0;

	virtual void onObjCountInc(const std::string& strClassName, int iSingleSize) = 0;

	virtual void onObjCountDec(const std::string& strClassName) = 0;
};

END_MINIAPR_NAMESPACE

#endif
