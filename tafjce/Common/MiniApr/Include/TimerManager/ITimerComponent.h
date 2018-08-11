/*********************************************************
*
*	名称: ITimerSink.h
*	作者: feiwu
*	时间: 2011-03-14
*	描述: 定时器接口类
*********************************************************/

#ifndef __ITIMER_COMPONENT_H__
#define __ITIMER_COMPONENT_H__


BEGIN_MINIAPR_NAMESPACE

class ITimerCallback
{
public:

	// 功能: 虚析构函数
	virtual ~ITimerCallback(){};

	// 功能: 定时器回调接口
	// 参数: [nEventId|in] 定时任务ID, 这个参数是setTimer的时候设置的
	virtual void onTimer(int nEventId) = 0;
};


#define IID_ITimerComponent	MAKE_RID('t', 'm', 'c', 'p')

class ITimerComponent:public IComponent
{
public:

	struct TimeNodeInfo:public Detail::EventHandle
	{
		TimeNodeInfo():pTimerSink(0), dwInterval(0), dwNextJeff(0),
			dwLastTickCount(0), nEventId(0),bIsHead(0), bDelete(0),
			pPre(0), pNext(0)
		{
		}
		
		ITimerCallback* pTimerSink;
		unsigned long	dwInterval;
		unsigned long	dwNextJeff;
		unsigned long	dwLastTickCount;
		int				nEventId;
		bool			bIsHead;
		bool			bDelete;
		
		TimeNodeInfo* pPre;
		TimeNodeInfo* pNext;

		std::string strReason;
	};

	typedef Detail::EventHandle::Proxy		TimerHandle;

	// 功能: 设置定时任务
	// 参数: [pSink|in] 定时回调对象
	// 参数: [lEventId] 任务ID
	// 参数: [nInterval] 定时间隔
	// 参数: [pszReason] 定时器描述信息，主要用于错误诊断
	// 参数: [dwDelayMicroSec] 延迟生效时间,毫秒
	// 返回: 定时任务Handle
	virtual TimerHandle setTimer(ITimerCallback* pSink, int lEventId, int nInterval, const char* pszReason, unsigned int dwDelayMicroSec = 0) = 0;

	// 功能: 取消定时器
	// 参数: [timerHandle] 定时任务的Handle
	virtual void killTimer(ITimerComponent::TimerHandle handle) = 0;

	// 功能: 运行定时器，一般在主循环运行
	virtual void runTimer() = 0;

	// 功能: 判断定时器是否存活
	virtual bool isTimerAlive(ITimerComponent::TimerHandle handle) = 0;
	
};

//extern "C" MINIAPR::ITimerComponent* createTimerComponent(Int32 nVersion);


END_MINIAPR_NAMESPACE

#endif

