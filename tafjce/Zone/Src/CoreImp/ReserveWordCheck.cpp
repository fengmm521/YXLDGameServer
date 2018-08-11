#include "CoreImpPch.h"
#include "ReserveWordCheck.h"

extern "C" IComponent* createReserveWordCheck(Int32 iVersion)
{
	return new ReserveWordCheck;
}


ReserveWordCheck::ReserveWordCheck()
{
	
}

ReserveWordCheck::~ReserveWordCheck()
{
	
}


bool ReserveWordCheck::initlize(const PropertySet& propSet)
{
	loadExtraReserverName(TABLENAME_ReservedWords, "ReservedWords");
	loadExtraReserverName(TABLENAME_Machine, "名字");
	loadExtraReserverName(TABLENAME_ArenaMonster, "名字");
	
	return true;
}


bool ReserveWordCheck::hasReserveWord(const string& strMsg)
{
	for(set<string>::iterator it = m_reserveWordList.begin(); it != m_reserveWordList.end(); it++)
	{
		if(strMsg.find(*it) != string::npos)
		{
			return true;
		}
	}
	
	return false;
}


void ReserveWordCheck::loadExtraReserverName(const string& strTableName, const string& strFiledName)
{
	ITable* pTable = getCompomentObjectManager()->findTable(strTableName);
	if(!pTable) return;

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		string strTmp = pTable->getString(i, strFiledName, "");
		m_reserveWordList.insert(strTmp);
	}
}


int ReserveWordCheck::getWordLength(const string& strMsg)
{
	const char* pszStr = strMsg.c_str();
	size_t dwLen = strMsg.size();
	
	int iCount = 0;
	int iCurCounter = 0;
	size_t i = 0;
	while(i < dwLen )
	{
		unsigned char val = pszStr[i];
	
		if(( val & 0x80) == 0)
		{ 	// 0xxxxxxx, ASCII
			iCount++;
			iCurCounter = 0;
			//TODO: check 0~9, a~z, A~Z, -_
			if ( !(( val >= '0' &&  val <='9'  ) || (val >= 'a' && val <= 'z' ) || 
				( val >= 'A' && val <= 'Z' )) )
			{
				return -1;
			}
		}
		else
		{
			if(( val & 0xC0) == 0x80)
			{ 	// 10xxxxxx                
				if(iCurCounter > 0)
				{
					iCurCounter--;
					i++;
				}
				else
				{
					return -3;
				}
			}
			else if( val < 0xF0)
			{ //110xxxxx ~ 1110xxxx, 只考虑UCS2
			    if( ((i+1)<dwLen) && (0xC2 == val) && ( 0xA0 ==((unsigned char)( pszStr[i+1]))))   // 排版的空格
                {
                	return -2;
			    }
				iCurCounter = val < 0xE0 ? 1 : 2;
				iCount++;
				i++;
			}
			else
			{
				return -4;
			}
			continue;
		}
		i++;
	}
	
	return iCount;
}


size_t ReserveWordCheck::calLength(const char* src) const
{
	size_t src_len = strlen(src);
	size_t destCapacity = 0;

	// while there is data in the source buffer, and space in the dest buffer
	for (size_t idx = 0; idx < src_len;)
	{
		char	cp;
		char	cu = src[idx++];

		if (cu < 0x80)
		{
			cp = (char)(cu);
		}
		else if (cu < 0xE0)
		{
			cp = ((cu & 0x1F) << 6);
			cp |= (src[idx++] & 0x3F);
		}
		else if (cu < 0xF0)
		{
			cp = ((cu & 0x0F) << 12);
			cp |= ((src[idx++] & 0x3F) << 6);
			cp |= (src[idx++] & 0x3F);
		}
		else
		{
			cp = ((cu & 0x07) << 18);
			cp |= ((src[idx++] & 0x3F) << 12);
			cp |= ((src[idx++] & 0x3F) << 6);
			cp |= (src[idx++] & 0x3F);
		}

		destCapacity++;
	}

	return destCapacity;
}




