#ifndef __REFCOUNT_AUTOPTR_H__
#define __REFCOUNT_AUTOPTR_H__

#include "Lock.h"

namespace cocos2d
{


template<typename T, bool autoDelete>
class RefcountAutoPtrRep
{
public:

	RefcountAutoPtrRep():m_object(NULL), m_nRefcount(1)
	{
	}

	~RefcountAutoPtrRep()
	{
	}

	void bind(T* pobject)
	{
		m_mutext.lock();
		m_nRefcount = 1;
		m_mutext.unlock();
		
		m_object = pobject;
	}

	T*get()
	{
		return m_object;
	}

	void reset()
	{
		m_object = NULL;
	}

	void incRefcount()
	{
		m_mutext.lock();
		m_nRefcount++;
		m_mutext.unlock();
		
	}
	
	void decRefcount()
	{
		m_mutext.lock();
		--m_nRefcount;
		m_mutext.unlock();
		
		
		if(0 == m_nRefcount)
		{
			if(autoDelete)
			{
				delete m_object;
				m_object = NULL;
			}
			delete this;
		}
	}


private:
	T*		m_object;
	int	m_nRefcount;
	Mutext m_mutext;
};


template<typename T, bool autoDelete = false>
class RefcountAutoPtr
{
public:

	RefcountAutoPtr():m_pImpData(NULL)
	{
	}

	bool isNULL(){return m_pImpData == NULL;}

	~RefcountAutoPtr()
	{
		if(m_pImpData)
		{
			m_pImpData->decRefcount();
			m_pImpData = NULL;
		}
	}

	void reset()
	{
		m_pImpData->reset();
	}

	void bind(T* pobject)
	{
		if(m_pImpData)
		{
			m_pImpData->decRefcount();
		}

		m_pImpData = new RefcountAutoPtrRep<T, autoDelete>;
		m_pImpData->bind(pobject);
	}

	void operator = (const RefcountAutoPtr& rhs)
	{
		if(m_pImpData)
		{
			m_pImpData->decRefcount();
		}

		m_pImpData = rhs.m_pImpData;
		if(m_pImpData)
		{
			m_pImpData->incRefcount();
		}
	}

	RefcountAutoPtr(const RefcountAutoPtr& rhs)
	{
		m_pImpData = rhs.m_pImpData;
		if(m_pImpData)
		{
			m_pImpData->incRefcount();
		}
	}


	T*get() const
	{
		if(!m_pImpData)
		{
			return NULL;
		}
		return m_pImpData->get();
	}

private:
	RefcountAutoPtrRep<T, autoDelete>*	m_pImpData;
};


class EventHandle
{
public:
	typedef RefcountAutoPtr<EventHandle> Proxy;
	EventHandle(){m_proxy.bind(this);}
	~EventHandle(){m_proxy.reset();}
	Proxy getEventHandle(){return m_proxy;}
private:
	Proxy m_proxy;
};

}

#endif

