#ifndef __FAST_PROPSET2_H__
#define __FAST_PROPSET2_H__

BEGIN_MINIAPR_NAMESPACE


class FastPropset2
{
public:

	typedef int PropertyKey;

	FastPropset2()
	{
		m_iSize = 2;
		m_pIntPropArray = new int[m_iSize];
		memset(m_pIntPropArray, 0, sizeof(int)*m_iSize);
		
		m_pIntTypeArray = new char[m_iSize];
		memset(m_pIntTypeArray, dtNull, m_iSize);
	}
	
	~FastPropset2()
	{
		delete []m_pIntPropArray;
		delete []m_pIntTypeArray;
	}

	PropType getType(int iKey) const
	{
		if(m_stringProp.find(iKey) != m_stringProp.end() )
		{
			return dtString;
		}
		
		if(m_Int64Prop.find(iKey) != m_Int64Prop.end() )
		{
			return dtInt64;
		}
		
		if(m_floatProp.find(iKey) != m_floatProp.end() )
		{
			return dtFloat;
		}
		
		if( (size_t)iKey >= (size_t)m_iSize)
		{		
			return dtNull;
		}
		
		return (PropType)m_pIntTypeArray[iKey];
	}

	inline void __attribute__((always_inline))  setInt(int iKey, int iValue)
	{
		if(iKey >= m_iSize)
		{
			expandSpace(iKey);
		}
		m_pIntPropArray[iKey] = iValue;
		m_pIntTypeArray[iKey] = dtInt;
	}

	void setString(int iKey, const char* pszValue)
	{
		m_stringProp[iKey] = pszValue;
	}

	void setFloat(int iKey, float fValue)
	{
		m_floatProp[iKey] = fValue;
	}

	void setInt64(int iKey, long  i64Value)
	{
		m_Int64Prop[iKey] = i64Value;
	}

	inline int __attribute__((always_inline)) getInt(int iKey,int iDef = 0) const
	{
		if( (size_t)iKey >= (size_t)m_iSize)
		{
			return iDef;
		}
		
		if(m_pIntTypeArray[iKey] != dtInt)
		{
				return iDef;
		}

		return m_pIntPropArray[iKey];
	}

	float getFloat(int iKey, float fDef) const
	{
		map<int, float>::const_iterator it = m_floatProp.find(iKey);
		if(it == m_floatProp.end() )
		{
			return fDef;
		}
		
		return it->second;
	}

	const char* getString(int iKey, const char* strDef) const
	{
		map<int, string>::const_iterator it = m_stringProp.find(iKey);
		if(it == m_stringProp.end() )
		{
			return strDef;
		}
		
		return it->second.c_str();
	}

	long getInt64(int iKey, long i64Def) const
	{
		map<int, long>::const_iterator it = m_Int64Prop.find(iKey);
		if(it == m_Int64Prop.end() )
		{
			return i64Def;
		}
		
		return it->second;
	}

	FastPropset2& operator += (const FastPropset2& rhs)
	{
		for(int i =0; i < rhs.m_iSize; i++)
		{
			if(rhs.checkInitPropType(i) )
			{
				setInt(i, rhs.getInt(i) );	
			}
		}
		
		for(map<int, string>::const_iterator it = rhs.m_stringProp.begin(); it != rhs.m_stringProp.end(); it++)
		{
			setString(it->first, it->second.c_str() );
		}
		
		for(map<int, float>::const_iterator it = rhs.m_floatProp.begin(); it != rhs.m_floatProp.end(); it++)
		{
			setFloat(it->first, it->second);
		}
		
		for(map<int, long>::const_iterator it = rhs.m_Int64Prop.begin(); it != rhs.m_Int64Prop.end(); it++)
		{
			setInt64(it->first, it->second);
		}
		
		return *this;
	}

	PropType getPropTypeByKey(int iKey) const
	{
		return (PropType)getType(iKey);
	}

	FastPropset2(const FastPropset2& rhs)
	{
		m_iSize = rhs.m_iSize;
		m_pIntPropArray = new int[m_iSize];
		memset(m_pIntPropArray, 0, sizeof(int)*m_iSize);
		
		m_pIntTypeArray = new char[m_iSize];
		memset(m_pIntTypeArray, 0, m_iSize);
		
		*this += rhs;
	}
	
	FastPropset2& operator = (const FastPropset2& rhs)
	{
		if(m_pIntPropArray) delete []m_pIntPropArray;
		if(m_pIntTypeArray) delete []m_pIntTypeArray;
			
		m_iSize = rhs.m_iSize;
		m_pIntPropArray = new int[m_iSize];
		memset(m_pIntPropArray, 0, sizeof(int)*m_iSize);
		
		m_pIntTypeArray = new char[m_iSize];
		memset(m_pIntTypeArray, 0, m_iSize);
		
		*this += rhs;

		return *this;
	}

	int getIntPropCount() const
	{
		return m_iSize;
	}

	bool checkInitPropType(int iKey) const
	{
		return m_pIntTypeArray[iKey] == dtInt;
	}

private:

	void expandSpace(int iKey)
	{
		assert(iKey >= 0);
		if((size_t)iKey >= (size_t)m_iSize)
		{
			int iResultSize = std::max(iKey+1, m_iSize*2);
			int* pNewArray = new int[iResultSize];
			char* pNewIntTypeArrat = new char[iResultSize];
			
			memset(pNewArray, 0, sizeof(int)*iResultSize);
			memcpy(pNewArray, m_pIntPropArray, sizeof(int)*m_iSize);
			
			memset(pNewIntTypeArrat, 0, iResultSize);
			memcpy(pNewIntTypeArrat, m_pIntTypeArray, m_iSize);
			
			m_iSize = iResultSize;
			
			delete []m_pIntPropArray;
			delete []m_pIntTypeArray;
			m_pIntPropArray = pNewArray;
			m_pIntTypeArray = pNewIntTypeArrat;
		}
	}

private:
	int* m_pIntPropArray;
	char* m_pIntTypeArray;
	map<int, string> m_stringProp;
	map<int, float> m_floatProp;
	map<int, long> m_Int64Prop;
	
	int m_iSize;
};

END_MINIAPR_NAMESPACE


#endif
