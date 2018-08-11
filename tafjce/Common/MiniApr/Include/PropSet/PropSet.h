/*********************************************************
*
*	名称: PropSet.h
*	作者: wuxf
*	时间: 2011-04-17
*	描述: 属性集类
*********************************************************/

#ifndef __PROPSET_H__
#define __PROPSET_H__

#include "MiniAprType.h"

enum PropType
{
	dtNull = 0,
	dtInt,
	dtString,
	dtFloat,
	dtInt64,
};

#include "FastPropset.h"
#include "FastPropset2.h"


BEGIN_MINIAPR_NAMESPACE

// 采用vector存储数据，插入和删除采用二分查找
template<class _KEY>
class _PropertySetT
{
public:
	
	typedef _KEY PropertyKey;
	struct KvPair
	{
		KvPair():dataKey(_KEY() ), dataType(dtNull){}
		KvPair(const KvPair& rhs)
		{
			dataType = rhs.dataType;
			dataKey = rhs.dataKey;
			if( (rhs.dataType == dtString) && (rhs.value.strValue) )
			{
				size_t sLen = strlen(rhs.value.strValue);
				value.strValue = new char[sLen + 1];
				memcpy(value.strValue, rhs.value.strValue, sLen);
				value.strValue[sLen] = '\0';
			}
			else if(rhs.dataType == dtInt)
			{
				value.nValue = rhs.value.nValue;
			}
			else if(rhs.dataType == dtFloat)
			{
				value.fValue = rhs.value.fValue;
			}
			else if(rhs.dataType == dtInt64)
			{
				value.nValue64 = rhs.value.nValue64;
			}
		}

		KvPair& operator = (const KvPair& rhs)
		{
			if(dtString == dataType)
			{
				delete []value.strValue;
				value.strValue = NULL;
			}
		
			dataType = rhs.dataType;
			dataKey = rhs.dataKey;
			if( (rhs.dataType == dtString) && (rhs.value.strValue) )
			{
				size_t sLen = strlen(rhs.value.strValue);
				value.strValue = new char[sLen + 1];
				memcpy(value.strValue, rhs.value.strValue, sLen);
				value.strValue[sLen] = '\0';
			}
			else if(rhs.dataType == dtInt)
			{
				value.nValue = rhs.value.nValue;
			}
			else if(rhs.dataType == dtFloat)
			{
				value.fValue = rhs.value.fValue;
			}
			else if(rhs.dataType == dtInt64)
			{
				value.nValue64 = rhs.value.nValue64;
			}

			return *this;
		}

		~KvPair()
		{
			if(dataType == dtString)
			{
				if(value.strValue)
				{
					delete []value.strValue;
				}
			}
		}
		
		bool operator <(const KvPair& hOther) const
		{
			return dataKey < hOther.dataKey;
		}

		bool operator == (const KvPair& hOther)const
		{
			return dataKey == hOther.dataKey;
		}


		PropertyKey dataKey;
		PropType dataType;
		union
		{
			Int32 nValue;
			char* strValue;
			float fValue;
			Int64 nValue64;
		}value;
	};

	typedef std::vector<KvPair>  PropList;

	_PropertySetT(){}

	~_PropertySetT()
	{
		m_PropValueList.clear();
	}

	Int8 getType(PropertyKey key)
	{
		KvPair element;
		element.dataKey = key;

		typename std::vector<KvPair>::iterator it = std::lower_bound(m_PropValueList.begin(), m_PropValueList.end(), element);
		if(it == m_PropValueList.end() )
		{
			return dtNull;
		}

		if(it->dataKey == key)
		{
			return it->dataType;
		}

		return dtNull;
	}

	void setInt(PropertyKey key, Int32 nValue)
	{
		KvPair element;
		element.dataKey = key;
		element.dataType = dtInt;
		element.value.nValue = nValue;
		addElementImp(element);
	}

	void setString(PropertyKey key, const char* pszValue)
	{
		KvPair element;
		element.dataKey = key;
		element.dataType = dtString;
		
		element.value.strValue = new char[strlen(pszValue) + 1];
		strcpy(element.value.strValue, pszValue);

		addElementImp(element);
	}

	void setFloat(PropertyKey key, float fValue)
	{
		KvPair element;
		element.dataKey = key;
		element.dataType = dtFloat;
		element.value.fValue = fValue;
		addElementImp(element);
	}

	void setInt64(PropertyKey key, Int64 nValue64)
	{
		KvPair element;
		element.dataKey = key;
		element.dataType = dtInt64;
		element.value.nValue64 = nValue64;
		addElementImp(element);
	}

	Int32 getInt(PropertyKey key,Int32 nDef = 0) const
	{
		const KvPair* pKvPairData = getElementImp(key);
		if(pKvPairData && (pKvPairData->dataType == dtInt) )
			return pKvPairData->value.nValue;

		return nDef;
	}

	float getFloat(PropertyKey key, float fDef) const
	{
		const KvPair* pKvPairData = getElementImp(key);
		if(pKvPairData && (pKvPairData->dataType == dtFloat) )
			return pKvPairData->value.fValue;

		return fDef;
	}

	const char* getString(PropertyKey key, const char* strDef) const
	{
		const KvPair* pKvPairData = getElementImp(key);
		if(pKvPairData && (pKvPairData->dataType == dtString) )
			return pKvPairData->value.strValue;

		return strDef;
	}

	Int64 getInt64(PropertyKey key, Int64 nDef64) const
	{
		const KvPair* pKvPairData = getElementImp(key);
		if(pKvPairData && (pKvPairData->dataType == dtInt64) )
			return pKvPairData->value.nValue64;

		return nDef64;
	}

	_PropertySetT& operator += (const _PropertySetT& rhs)
	{
		size_t size = rhs.m_PropValueList.size();
		for(size_t i = 0; i < size; i++)
		{
			addElementImp(rhs.m_PropValueList[i]);
		}

		return *this;
	}

	size_t getPropCount()const {return m_PropValueList.size();}

	PropertyKey getPropKey(size_t pos) const
	{
		assert(pos < m_PropValueList.size() );
		return m_PropValueList[pos].dataKey;
	}

	PropType getPropType(size_t pos) const
	{
		assert(pos < m_PropValueList.size() );
		return m_PropValueList[pos].dataType;
	}

	PropType getPropTypeByKey(PropertyKey key) const
	{
		const KvPair* pKvPairData = getElementImp(key);
		if(pKvPairData)
		{
			return pKvPairData->dataType;
		}

		return dtNull;
	}

	_PropertySetT(const _PropertySetT& rhs)
	{
		*this += rhs;
	}

private:

	void addElementImp(const KvPair& element)
	{
		typename PropList::iterator it = std::lower_bound(m_PropValueList.begin(), m_PropValueList.end(), element);
		if(it != m_PropValueList.end() )
		{
			if(it->dataKey == element.dataKey)
			{
				*it = element;
				return;
			}
		}
		m_PropValueList.insert(it, element);
	}

	const KvPair* getElementImp(PropertyKey key) const
	{
		KvPair tmpData;
		tmpData.dataKey = key;
		typename PropList::const_iterator it = std::lower_bound(m_PropValueList.begin(), m_PropValueList.end(), tmpData);
		if(it == m_PropValueList.end() )
		{
			return NULL;
		}
		
		if(it->dataKey == key)
		{
			return &(*it);
		}

		return NULL;
	}
	
	PropList	m_PropValueList;
};



typedef FastPropset2  PropertySet;
typedef _PropertySetT<std::string>  PropertySet_S;

END_MINIAPR_NAMESPACE

#endif

