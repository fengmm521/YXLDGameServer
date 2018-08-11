#include "MiniAprPch.h"
#include "TimerManager.h"

using namespace MINIAPR;

extern "C" MINIAPR::ITimerComponent* createTimerComponent(Int32 nVersion)
{
	return new TimerManager;
}


#define LIST_FOR_EACH_ENTRY_SAFE(node, next, head)\
for(node = head->pNext, next = node->pNext; \
node != head; \
node = next, next = node->pNext)

TimerManager::TimerManager():m_dwLastJeff(0), m_bEnabled(false)
{
	for(int i = 0; i <TVR_SIZE; i++)
	{
		m_szLevel1[i].pPre = m_szLevel1[i].pNext = &m_szLevel1[i];
		m_szLevel1[i].bIsHead = true;
		m_szLevel1[i].bDelete = false;
	}

	for(int i = 0; i <TVN_SIZE; i++)
	{
		m_szLevel2[i].pPre = m_szLevel2[i].pNext = &m_szLevel2[i];
		m_szLevel3[i].pPre = m_szLevel3[i].pNext = &m_szLevel3[i];
		m_szLevel4[i].pPre = m_szLevel4[i].pNext = &m_szLevel4[i];
		m_szLevel5[i].pPre = m_szLevel5[i].pNext = &m_szLevel5[i];

		m_szLevel2[i].bIsHead = true;
		m_szLevel3[i].bIsHead = true;
		m_szLevel4[i].bIsHead = true;
		m_szLevel5[i].bIsHead = true;

		m_szLevel2[i].bDelete = false;
		m_szLevel3[i].bDelete = false;
		m_szLevel4[i].bDelete = false;
		m_szLevel5[i].bDelete = false;
	}
}

TimerManager::~TimerManager()
{

}

void TimerManager::_addTail(TimeNodeInfo* pHeadNode, TimeNodeInfo* pItemNode)
{
	_addItem(pHeadNode->pPre, pHeadNode, pItemNode);
}

void TimerManager::_addItem(TimeNodeInfo* pPreNode, TimeNodeInfo* pNextNode, TimeNodeInfo* pItemNode)
{
	pNextNode->pPre = pItemNode;
	pPreNode->pNext = pItemNode;

	pItemNode->pPre = pPreNode;
	pItemNode->pNext = pNextNode;
}

void TimerManager::_replaceInit(TimeNodeInfo* pHead, TimeNodeInfo* pReplaceHead)
{
	pReplaceHead->pNext = pHead->pNext;
	pReplaceHead->pNext->pPre = pReplaceHead;
	pReplaceHead->pPre = pHead->pPre;
	pReplaceHead->pPre->pNext = pReplaceHead;

	pHead->pNext = pHead;
	pHead->pPre = pHead;
}

TimerManager::TimerHandle TimerManager::setTimer(ITimerCallback* pSink, int lEventId, 
	int nInterval, const char* pszReason, unsigned int dwDelayMicroSec)
{
	TimeNodeInfo* pNodeInfo = new TimeNodeInfo;
	pNodeInfo->pTimerSink = pSink;
	pNodeInfo->dwInterval = nInterval;
	pNodeInfo->dwNextJeff = 0;
	pNodeInfo->nEventId = lEventId;
	pNodeInfo->dwLastTickCount = m_timeHelp.getMillonSecond() + dwDelayMicroSec;
	pNodeInfo->bDelete = false;
	pNodeInfo->strReason = pszReason;

	if(!m_bEnabled)
	{
		m_bEnabled = true;
		m_dwLastJeff = m_timeHelp.getCurrentJeff();
	}
	
	pNodeInfo->dwNextJeff = m_timeHelp.makeJeff(pNodeInfo->dwLastTickCount + pNodeInfo->dwInterval);
	pNodeInfo->dwLastTickCount += pNodeInfo->dwInterval;

	_addTimer(pNodeInfo);
	return pNodeInfo->getEventHandle();
}

void TimerManager::_addTimer(TimeNodeInfo* pNodeInfo)
{
	int nDisInt = pNodeInfo->dwNextJeff - m_dwLastJeff;
	
	TimeNodeInfo* pHeadNode = NULL;
	if(nDisInt < 0)
	{
		unsigned int dwPos = m_dwLastJeff & TVR_MASK;
		pHeadNode = &(m_szLevel1[dwPos]);
	}
	else if(nDisInt < TVR_SIZE)
	{
		unsigned int dwPos = pNodeInfo->dwNextJeff & TVR_MASK;
		pHeadNode = &(m_szLevel1[dwPos]);
	}
	else if( (nDisInt < 1<<(TVR_BITS + TVN_BITS) ) )
	{
		unsigned int dwPos = (pNodeInfo->dwNextJeff >> TVR_BITS) & TVN_MASK;
		pHeadNode = &(m_szLevel2[dwPos]);
	}
	else if( (nDisInt < 1<<(TVR_BITS + 2 * TVN_BITS) ) )
	{
		unsigned int dwPos = (pNodeInfo->dwNextJeff >> (TVR_BITS + TVN_BITS) ) & TVN_MASK;
		pHeadNode = &(m_szLevel3[dwPos]);
	}
	else if( (nDisInt < 1<<(TVR_BITS + 3 * TVN_BITS) ) )
	{
		unsigned int dwPos = (pNodeInfo->dwNextJeff >> (TVR_BITS + 2 * TVN_BITS) ) & TVN_MASK;
		pHeadNode = &(m_szLevel4[dwPos]);
	}
	else
	{
		unsigned int dwPos = (pNodeInfo->dwNextJeff >> (TVR_BITS + 3 * TVN_BITS) ) & TVN_MASK;
		pHeadNode = &(m_szLevel5[dwPos]);
	}
	
	_addTail(pHeadNode, pNodeInfo);
}

void TimerManager::killTimer(TimerManager::TimerHandle handle)
{
	TimeNodeInfo* pNodeInfo = static_cast<TimeNodeInfo*>(handle.get() );
	if(!pNodeInfo) return;
	
	pNodeInfo->bDelete = true;
}

bool TimerManager::isTimerAlive(TimerManager::TimerHandle handle)
{
	TimeNodeInfo* pNodeInfo = static_cast<TimeNodeInfo*>(handle.get() );
	if(!pNodeInfo) return false;

	return !(pNodeInfo->bDelete);
}


unsigned int TimerManager::_cascade(TimeNodeInfo* nodeVec,  unsigned int dwIndex)
{
	TimeNodeInfo* pHeadNode = &(nodeVec[dwIndex]);
	
	TimeNodeInfo workHead;
	_replaceInit(pHeadNode, &workHead);
	
	TimeNodeInfo* pWordNode = NULL;
	TimeNodeInfo* tmpNode = NULL;
	LIST_FOR_EACH_ENTRY_SAFE(pWordNode, tmpNode, (&workHead) )
	{
		_addTimer(pWordNode);
	}

	return dwIndex;
}

void TimerManager::runTimer()
{
	unsigned long dwCurrentJeff = m_timeHelp.getCurrentJeff();
	if(m_bEnabled)
	{
		for(unsigned long i = m_dwLastJeff; i < dwCurrentJeff; i++)
		{
			_runTimer(i);
			m_dwLastJeff = i + 1;
		}
	}
}

void TimerManager::_runTimer(unsigned long dwJeff)
{
	unsigned long dwIndex = dwJeff & TVR_MASK;

	if(!dwIndex && (!_cascade(m_szLevel2, (dwJeff>>TVR_BITS)& TVN_MASK) ) && 
		(!_cascade(m_szLevel3, (dwJeff>> (TVR_BITS + TVN_BITS))& TVN_MASK) ) && 
		(!_cascade(m_szLevel4, (dwJeff>> (TVR_BITS + 2 * TVN_BITS))& TVN_MASK) ) )
	{
		_cascade(m_szLevel5, (dwJeff>> (TVR_BITS + 3 * TVN_BITS))& TVN_MASK);
	}

	TimeNodeInfo* pHeadNode = &(m_szLevel1[dwIndex]);
	TimeNodeInfo workHead;
	_replaceInit(pHeadNode, &workHead);
	
	TimeNodeInfo* pWordNode = NULL;
	TimeNodeInfo* tmpNode = NULL;
	LIST_FOR_EACH_ENTRY_SAFE(pWordNode, tmpNode, (&workHead) )
	{
		if(pWordNode->bDelete)
		{
			delete pWordNode;
			continue;
		}

		pWordNode->pTimerSink->onTimer(pWordNode->nEventId);
		
		pWordNode->dwNextJeff = m_timeHelp.makeJeff(pWordNode->dwLastTickCount + pWordNode->dwInterval);
		pWordNode->dwLastTickCount += pWordNode->dwInterval;
		_addTimer(pWordNode);
	}
}
