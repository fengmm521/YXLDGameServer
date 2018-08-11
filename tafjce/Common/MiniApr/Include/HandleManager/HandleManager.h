#ifndef __HANDLE_MANAGER_H
#define __HANDLE_MANAGER_H

BEGIN_MINIAPR_NAMESPACE

template <class TAG>
class Handle
{
public:
	enum
	{
		en_HandleIndexBits = 22,
		en_HandleMagicBits = 10,

		en_MaxIndex = (1 << en_HandleIndexBits) - 1,
		en_MaxMagic = (1 << en_HandleMagicBits) - 1,
	};

private:
	union
	{
		struct
		{
			Uint32 m_dwMagic:en_HandleMagicBits;
			Uint32 m_dwIndex:en_HandleIndexBits;
		}m_data;
		Uint32 m_dwHandle;
	};

public:

	Handle(Uint32 dwHandle):m_dwHandle(dwHandle)
	{}
	
	Handle():m_dwHandle(0)
	{
	}
	
	void init(Uint32 nIndex, Uint32 nMaigcNum)
	{
		assert(nIndex <= en_MaxIndex);
		assert(nMaigcNum <= en_MaxMagic);
		m_data.m_dwIndex = nIndex;
		m_data.m_dwMagic = nMaigcNum;
	}

	operator unsigned int ()const
	{
		return m_dwHandle;
	}

	bool isNull()
	{
		return m_dwHandle == 0;
	}
	
	Uint32 getHandle()const
	{
		return m_dwHandle;
	}
	
	Uint32 getMagicNum()
	{
		return m_data.m_dwMagic;
	}

	Uint32 getIndex()
	{
		return m_data.m_dwIndex;
	}

	bool operator == (const Handle& rhs) const
	{
		return m_dwHandle == rhs.getHandle();
	}

	bool operator != (const Handle& rhs) const
	{
		return !(*this == rhs);
	}
    bool operator < (const Handle& other) const
    {
        return (m_dwHandle < other.getHandle());
    }
};

template<class DATA>
class HandleManager
{
public:
	HandleManager():m_nError("只有指针类才能使用HandleManager")
	{}

private:
	int m_nError;
	std::vector<DATA> m_dataList;
};

template<class DATA>
class HandleManager<DATA*>
{
public:
	HandleManager():m_MaicNum(1){}

	~HandleManager(){}
	DATA** Aquire(Handle<DATA*>& handle)
	{
		Uint32 index = 0;
		if(m_freeList.empty() )
		{
			index = (Uint32)m_magicList.size();
			assert(index <= (Uint32)Handle<DATA*>::en_MaxIndex);
			handle.init(index, m_MaicNum++);
			if(m_MaicNum > Handle<DATA*>::en_MaxMagic)
			{
				m_MaicNum = 1;
			}
			m_dataList.push_back(NULL);
			m_magicList.push_back(handle.getMagicNum() );
		}
		else
		{
			index = m_freeList.back();
			handle.init(index, m_MaicNum++);
			if(m_MaicNum > Handle<DATA*>::en_MaxMagic)
			{
				m_MaicNum = 1;
			}
			m_freeList.pop_back();
			m_magicList[index] = handle.getMagicNum();
		}
		return  &m_dataList[index];
	}
	
	
	DATA** AcuireFromIndex(Handle<DATA*> handle)
	{
		if(handle.getIndex() >= m_dataList.size() )
		{
			for(size_t i = m_dataList.size(); i < handle.getIndex(); i++)
			{
				m_magicList.push_back(0);
				m_freeList.push_back(i);
				m_dataList.push_back(NULL);
			}
			m_dataList.push_back(NULL);
			m_magicList.push_back(handle.getMagicNum() );
		}
		else
		{
			// 首先判断是否则FreeList中
			FreeList::iterator it = find(m_freeList.begin(), m_freeList.end(), handle.getIndex() );
			if(it != m_freeList.end() )
			{
				m_freeList.erase(it);
				m_dataList[handle.getIndex()] = NULL;
				m_magicList[handle.getIndex()] = handle.getMagicNum();
			}
			else
			{
				assert(false);
				//MINIAPR_ERROR("AcuireFromIndex 错误，Handle已经被占用!!");
			}
		}

		return &m_dataList[handle.getIndex() ];
	}

	void Release(Handle<DATA*> handle)
	{
		Uint32 index = handle.getIndex();
		
		if( (index >= m_dataList.size()) || (m_magicList[index] != handle.getMagicNum()) )
		{
			assert(false);
			return;
		}
		
		m_dataList[index] = NULL;
		m_magicList[index] = 0;
		m_freeList.push_back(index);
	}

	DATA* getDataPoint(Handle<DATA*> handle)
	{
		if(handle.isNull() )
			return NULL;

		Uint32 index = handle.getIndex();
		if( (index >= m_dataList.size() ) || (m_magicList[index] != handle.getMagicNum() ) )
		{
			return NULL;
		}

		if(0 == handle.getMagicNum() )
		{
			return NULL;
		}

		return *(m_dataList.begin() + index);
	}

private:
	typedef std::vector<DATA*>		UserDataList;
	typedef std::vector<Uint32>		MagicList;
	typedef std::vector<Uint32>		FreeList;	

	UserDataList	m_dataList;
	MagicList		m_magicList;
	FreeList		m_freeList;
	Uint32			m_MaicNum;
};

END_MINIAPR_NAMESPACE

#endif
