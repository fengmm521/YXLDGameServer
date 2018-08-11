#ifndef __FAST_PROPSET_H__
#define __FAST_PROPSET_H__


BEGIN_MINIAPR_NAMESPACE

class FastPropset
{
public:

	typedef int PropertyKey;

	struct FastKVPair
	{
		
		FastKVPair():cType(dtNull)
		{
		}

		FastKVPair(const FastKVPair& rhs)
		{
			cType = dtNull;
			copyFrom(rhs);
		}

		FastKVPair& operator =(const FastKVPair& rhs)
		{
			copyFrom(rhs);
			return *this;
		}

		~FastKVPair()
		{
			if(dtString == cType)
			{
				cType = dtNull;
				delete []value.strValue;
			}
		}

		void copyFrom(const FastKVPair& rhs)
		{
			if(dtString == cType)
			{
				delete []value.strValue;
				value.strValue = NULL;
				cType = dtNull;
			}
		
			cType = rhs.cType;
			switch(cType)
			{
				case dtNull:
					break;

				case dtInt:
					value.iValue = rhs.value.iValue;
					break;

				case dtString:
					{
						size_t sLen = strlen(rhs.value.strValue);
						value.strValue = new char[sLen + 1];
						memcpy(value.strValue, rhs.value.strValue, sLen);
						value.strValue[sLen] = '\0';
					}
					break;

				case dtFloat:
					value.fValue = rhs.value.fValue;
					break;

				case dtInt64:
					value.i64Value = rhs.value.i64Value;
					break;
			}
		}

		PropType cType;
		union
		{
			Int32 iValue;
			char* strValue;
			float fValue;
			Int64 i64Value;
		}value;
	};

	FastPropset()
	{
	}
	
	~FastPropset()
	{
	}

	Int8 getType(int iKey)
	{
		if( (size_t)iKey >= m_propList.size() )
		{
			return dtNull;
		}

		return m_propList[iKey].cType;
	}

	void clearItemData(int iKey)
	{
		if(m_propList.size() <= (size_t)iKey)
		{
			return;
		}

		if(dtString == m_propList[iKey].cType)
		{
			delete []m_propList[iKey].value.strValue;
			m_propList[iKey].value.strValue = NULL;
			m_propList[iKey].cType = dtNull;
		}
	}

	void setInt(int iKey, Int32 iValue)
	{
		expandSpace(iKey);
		clearItemData(iKey);

		m_propList[iKey].cType = dtInt;
		m_propList[iKey].value.iValue = iValue;
	}

	void setString(int iKey, const char* pszValue)
	{
		expandSpace(iKey);
		clearItemData(iKey);
		
		m_propList[iKey].cType = dtString;

		size_t sLen = strlen(pszValue);
		m_propList[iKey].value.strValue = new char[sLen+1];
		memcpy(m_propList[iKey].value.strValue, pszValue, sLen);
		m_propList[iKey].value.strValue[sLen] = '\0';
	}

	void setFloat(int iKey, float fValue)
	{
		expandSpace(iKey);
		clearItemData(iKey);
		m_propList[iKey].cType = dtFloat;
		m_propList[iKey].value.fValue = fValue;
	}

	void setInt64(int iKey, Int64 i64Value)
	{
		expandSpace(iKey);
		clearItemData(iKey);
		m_propList[iKey].cType = dtInt64;
		m_propList[iKey].value.i64Value = i64Value;
	}

	Int32 getInt(int iKey,Int32 iDef = 0) const
	{
		if( (size_t)iKey >= m_propList.size() )
		{
			return iDef;
		}

		if(m_propList[iKey].cType != dtInt)
		{
			//assert(false);
			return iDef;
		}

		return m_propList[iKey].value.iValue;
	}

	float getFloat(int iKey, float fDef) const
	{
		if( (size_t)iKey >= m_propList.size() )
		{
			return fDef;
		}

		if(m_propList[iKey].cType != dtFloat)
		{
			//assert(false);
			return fDef;
		}

		return m_propList[iKey].value.fValue;
	}

	const char* getString(int iKey, const char* strDef) const
	{
		if( (size_t)iKey >= m_propList.size() )
		{
			return strDef;
		}

		if(m_propList[iKey].cType != dtString)
		{
			//assert(false);
			return strDef;
		}

		return m_propList[iKey].value.strValue;
	}

	Int64 getInt64(int iKey, Int64 i64Def) const
	{
		if( (size_t)iKey >= m_propList.size() )
		{
			return i64Def;
		}

		if(m_propList[iKey].cType != dtInt64)
		{
			//assert(false);
			return i64Def;
		}

		return m_propList[iKey].value.i64Value;
	}

	FastPropset& operator += (const FastPropset& rhs)
	{
		size_t size = rhs.m_propList.size();
		for(size_t i = 0; i < size; i++)
		{
			switch(rhs.m_propList[i].cType)
			{
				case dtNull:
					break;

				case dtInt:
					setInt(i, rhs.m_propList[i].value.iValue);
					break;

				case dtString:
					setString(i, rhs.m_propList[i].value.strValue);
					break;

				case dtFloat:
					setFloat(i, rhs.m_propList[i].value.fValue);
					break;

				case dtInt64:
					setInt64(i, rhs.m_propList[i].value.i64Value);
					break;
			}
		}

		return *this;
	}

	size_t getPropCount()const
	{
		return m_propList.size();
	}

	PropType getPropType(int iKey) const
	{
		if( (size_t)iKey >= m_propList.size() )
		{
			return dtNull;
		}

		return m_propList[iKey].cType;
	}

	int getPropKey(int iPos) const
	{
		return iPos;
	}

	PropType getPropTypeByKey(int iKey) const
	{
		return getPropType(iKey);
	}

	FastPropset(const FastPropset& rhs)
	{
		*this += rhs;
	}

private:

	void expandSpace(int iKey)
	{
		assert(iKey >= 0);
		if((size_t)iKey >= m_propList.size() )
		{
			m_propList.resize((size_t)iKey+1);
		}
	}

private:

	std::vector<FastKVPair>	m_propList;
};

END_MINIAPR_NAMESPACE

#endif
