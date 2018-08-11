/*********************************************************
*
*	名称: EventServer.h
*	作者: wuxf
*	时间: 2009-03-03
*	描述: 事件发生器类
*********************************************************/
#ifndef _EVENT_SERVER_H__
#define _EVENT_SERVER_H__

#include <set>
#include <map>
#include <string>
#include <iostream>
#include "Delegate.h"
#include "PropSet/PropSet.h"

BEGIN_MINIAPR_NAMESPACE


struct EventArgs
{
	EventArgs(){}
	~EventArgs(){}
	PropertySet_S	context;
};





template <class ResultType, class ArgsType>
class DelegateListHold
{
public:

	typedef typename TypeAt<ArgsType, 0>::Result	P1;
	typedef std::vector<Detail::Delegate<ResultType, ArgsType> >  DelegateList;

	DelegateListHold(int iEventID):m_iEventID(iEventID), m_bInUse(false)
	{
		
		m_listHandle.bind(new DelegateList);
	}

	DelegateListHold(const DelegateListHold& rhs)
	{
		m_listHandle = rhs.m_listHandle;
		m_iEventID = rhs.m_iEventID;
		m_bInUse = rhs.m_bInUse;
	}

	~DelegateListHold()
	{
	}

	void addDelegate(const Detail::Delegate<ResultType, ArgsType>& delegate)
	{
		if(m_bInUse)
		{
			RefcountAutoPtr<DelegateList, true> tmpListHandle;
			tmpListHandle.bind(new DelegateList);
			*(tmpListHandle.get() ) = *(m_listHandle.get() );
			m_listHandle = tmpListHandle;
		}
		
		DelegateList* pList = m_listHandle.get();
		assert(pList);
		typename DelegateList::iterator it = std::lower_bound(pList->begin(), pList->end(), delegate);
		if(it != pList->end() )
		{
			if(*it == delegate)
			{
				FDLOG("ViewError")<<"register same event:"<<m_iEventID<<endl;
				return;
			}
		}
		pList->insert(it, delegate);
	}

	void delDelegate(const Detail::Delegate<ResultType, ArgsType>& delegate)
	{
		if(m_bInUse)
		{
			RefcountAutoPtr<DelegateList, true> tmpListHandle;
			tmpListHandle.bind(new DelegateList);
			*(tmpListHandle.get() ) = *(m_listHandle.get() );
			m_listHandle = tmpListHandle;
		}

		DelegateList* pList = m_listHandle.get();
		assert(pList);
		typename DelegateList::iterator it = std::lower_bound(pList->begin(), pList->end(), delegate);
		if(it != pList->end() )
		{
			if(*it == delegate)
			{
				pList->erase(it);
			}
		}
		
	}
	
	void setEvent(P1& args)
	{
		m_bInUse = true;
		RefcountAutoPtr<DelegateList, true> tmpHandle = m_listHandle;
		DelegateList* pList = tmpHandle.get();
		if(!pList)
		{
			m_bInUse = false;
			return;
		}
		
		for(size_t i = 0; i < pList->size(); i++)
		{
			(*pList)[i](args);
		}
		m_bInUse = false;
	}

private:
	RefcountAutoPtr<DelegateList, true> m_listHandle;
	int m_iEventID;
	bool m_bInUse;
};


template <class ResultType, class ArgsType>
class  _EventServer
{
public:

	typedef typename TypeAt<ArgsType, 0>::Result	P1;
	typedef std::map<Int32, DelegateListHold<ResultType,ArgsType > >  MapDelegateList;

	_EventServer(){}
	~_EventServer(){}

	template<class T, class MemFunType>
		void subscribeEvent(Int32 nEventId, T* pObj, MemFunType pFun)
	{
		typename MapDelegateList::iterator it = m_mapDelegate.find(nEventId);	
		if(it == m_mapDelegate.end() )
		{
			DelegateListHold<ResultType, ArgsType> tmpHold(nEventId);
			m_mapDelegate.insert(std::make_pair(nEventId, tmpHold) );
			it = m_mapDelegate.find(nEventId);
			assert(it != m_mapDelegate.end() );
		}

		DelegateListHold<ResultType, ArgsType>& listHold = it->second;
		listHold.addDelegate(Detail::Delegate<ResultType, ArgsType>(pObj, pFun) );
	}

	template<class T, class MemFunType>
		void unsubscribeEvent(Int32 nEventId, T* pObj, MemFunType pFun)
	{
		typename MapDelegateList::iterator it = m_mapDelegate.find(nEventId);	
		if(it == m_mapDelegate.end() )
		{
			return;
		}

		DelegateListHold<ResultType, ArgsType>& listHold = it->second;
		listHold.delDelegate(Detail::Delegate<ResultType, ArgsType>(pObj, pFun) );
	}

	void setEvent(Int32 nEventId, P1& args)
	{
		typename MapDelegateList::iterator it = m_mapDelegate.find(nEventId);	
		if(it == m_mapDelegate.end() )
		{
			return;
		}

		DelegateListHold<ResultType, ArgsType>& listHold = it->second;
		listHold.setEvent(args);
	}

private:
	
	MapDelegateList m_mapDelegate;
};

typedef  _EventServer<void, APR_TYPELIST_1(EventArgs)>  EventServer;

END_MINIAPR_NAMESPACE


#endif

