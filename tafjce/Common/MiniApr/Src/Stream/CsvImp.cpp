#include "MiniAprPch.h"
#include "CsvImp.h"
#include "Stream/DbcFile.h"
#include "util/tc_encoder.h"

using namespace MINIAPR;
using namespace std;

#define MAX_CSVROW_SIZE		(4096*2)

CsvImpApr::CsvImpApr():m_parseState(en_CsvState_Begin)
{}

bool CsvImpApr::open(const char* strFileName)
{	
	m_strFileName = strFileName;
	ifstream ifs (strFileName, ifstream::binary);
	if(!ifs.good() ) return false;

	ifs.seekg(0, ios::end);
	size_t sFileLen = ifs.tellg();
	ifs.seekg (0, ios::beg);
	char* pFileData = new char[sFileLen];
	ifs.read(pFileData, sFileLen);
	ifs.close();
	string strCsvData;
	strCsvData.assign(pFileData, sFileLen);
	if(strCsvData[strCsvData.size() - 1] != '\n')
	{
		strCsvData += "\r\n";
	}
	
	delete []pFileData;

	size_t sBeginPos = 0;
	vector<vector<string> > orgRowList;
	orgRowList.resize(1);
	while(sBeginPos < sFileLen)
	{
		if(!parseCsvData(strCsvData, sBeginPos, orgRowList) )
		{
			return false;
		}
	}

	printf("csv file name=%s\n",getFileName().c_str());

	orgRowList.pop_back();
	fillCsvData(orgRowList);

	// 建立field索引
	for(size_t i = 0; i < m_fieldData.size(); i++)
	{
		m_fieldMap[m_fieldData[i].strFieldName] = m_fieldData[i];
	}
	
	return true;
}

string CsvImpApr::getFileName()
{
    return m_strFileName;
}

void CsvImpApr::fillCsvData(vector<vector<string> >& orgRowList)
{
	assert(orgRowList.size() > 0);

	// 解析数据档
	int nState = en_CsvState_Begin;
	size_t filedTypeRow = 0;
	m_csvData.reserve(orgRowList.size() );
	for(size_t i = 0; i < orgRowList.size(); i++)
	{
		vector<string>& singleRow = orgRowList[i];
		assert(singleRow.size() > 0);

		// 跳过注释
		if((en_CsvState_Begin == nState) || (en_CsvState_Comment ==nState ) )
		{
			if( (singleRow[0].substr(0, 2) == "##") )
			{
				nState = en_CsvState_Comment;
				continue;
			}

			nState = en_CsvState_FieldName;
		}

		if(en_CsvState_FieldName == nState)
		{
			for(size_t filedIndex = 0; filedIndex < singleRow.size(); filedIndex++)
			{
				CsvField tmpField;
				tmpField.csvItemType = CsvFile::en_CsvType_Null;
				tmpField.strFieldName = singleRow[filedIndex];
				tmpField.iIndex = (Int32)m_fieldData.size();
				printf("fieldname set:%d,%d,%s\n",int(filedIndex),tmpField.iIndex,tmpField.strFieldName.c_str());
				m_fieldData.push_back(tmpField);
			}
			nState = en_CsvState_FieldDef;
			continue;
		}

		if(en_CsvState_FieldDef == nState)
		{
			filedTypeRow = i;
			assert(singleRow.size() == m_fieldData.size() );
			for(size_t filedIndex = 0; filedIndex < singleRow.size(); filedIndex++)
			{
				if(singleRow[filedIndex] == "int")
				{
					m_fieldData[filedIndex].csvItemType = CsvFile::en_CsvType_Int;
				}
				else if(singleRow[filedIndex] == "string")
				{
					m_fieldData[filedIndex].csvItemType = CsvFile::en_CsvType_String;
				}
				else if(singleRow[filedIndex] == "utf8string")// 不独立新的类型了
				{
					m_fieldData[filedIndex].csvItemType = CsvFile::en_CsvType_String;
				}
				else if(singleRow[filedIndex] == "float")
				{
					m_fieldData[filedIndex].csvItemType = CsvFile::en_CsvType_Float;
				}
			}
			nState = en_CsvState_DataReading;
			continue;
		}

		if(en_CsvState_DataReading == nState)
		{
			
			if(singleRow.size() != m_fieldData.size())
			{
				printf("%s,%s,%d,%d\n",singleRow[0].c_str(),m_fieldData[m_fieldData.size()-1].strFieldName.c_str(),singleRow.size(),m_fieldData.size() );
				for(size_t filedIndex = 0; filedIndex < singleRow.size(); filedIndex++)
				{
					printf("%d,%s\n",int(filedIndex),singleRow[filedIndex].c_str());
				}
			}
			
			assert(singleRow.size() == m_fieldData.size() );
			m_csvData.push_back(CsvRow() );
			for(size_t filedIndex = 0; filedIndex < singleRow.size(); filedIndex++)
			{
				CsvItem tmpItem;
				tmpItem.csvItemType = m_fieldData[filedIndex].csvItemType;
				
				if(orgRowList[filedTypeRow][filedIndex] == "utf8string")
				{
					tmpItem.strItemData = TC_Encoder::gbk2utf8(singleRow[filedIndex]);
				}
				else
				{
					tmpItem.strItemData = singleRow[filedIndex];
				}
				
				m_csvData.back().push_back(tmpItem);
			}
			continue;
		}
	}
}


bool CsvImpApr::parseCsvData(const string& strCsvData, size_t& beginPos, vector<vector<string> >& orgRowList)
{
	if(strCsvData[beginPos] == '\"')
	{
		size_t tmpPos = beginPos+1;
		while(true)
		{
			tmpPos = strCsvData.find("\"", tmpPos);
			if( ( (tmpPos+1) < strCsvData.size() ) && (strCsvData[tmpPos+1] == '\"') )
			{
				tmpPos = tmpPos+2;
				continue;
			}
			break;
		}

		string strItem = strCsvData.substr(beginPos+1, tmpPos-beginPos-1);
		
		size_t sFindPos = strItem.find("\"\"");
		while(sFindPos != string::npos)
		{
			strItem.replace(sFindPos, 2, "\"");
			sFindPos = strItem.find("\"\"", sFindPos + 1);
		}
		orgRowList.back().push_back(strItem);
		//cout<<strItem<<"|";
		beginPos = tmpPos;
		size_t sNextLinePos = strCsvData.find("\n", beginPos);
		size_t sNextFieldPos = strCsvData.find(",", beginPos);
		if(sNextLinePos < sNextFieldPos)
		{
			orgRowList.push_back(vector<string>() );
			orgRowList.back().reserve(10);
			beginPos = sNextLinePos + 1;
			//cout<<"*"<<endl;
		}
		else
		{
			beginPos = sNextFieldPos + 1;
		}
	}
	else
	{
		size_t sNextLinePos = strCsvData.find("\n", beginPos);
		size_t sNextFieldPos = strCsvData.find(",", beginPos);
		if(sNextLinePos < sNextFieldPos)
		{
			string strItem = strCsvData.substr(beginPos, sNextLinePos-beginPos-1);
			orgRowList.back().push_back(strItem);
			orgRowList.push_back(vector<string>() );
			orgRowList.back().reserve(10);
			beginPos = sNextLinePos + 1;
			//cout<<strItem<<"*"<<endl;
		}
		else
		{
			string strItem = strCsvData.substr(beginPos, sNextFieldPos-beginPos);
			orgRowList.back().push_back(strItem);
			beginPos = sNextFieldPos + 1;
			//cout<<strItem<<"|";
		}
	}
	
	return true;
}

Int32 CsvImpApr::getRecordCount()
{
	return (Int32)m_csvData.size();
}

Int32 CsvImpApr::getFieldCount()
{
	return (Int32)m_fieldData.size();
}

Int32 CsvImpApr::getFieldIndex(const std::string& strFieldName)
{
	std::map<string, CsvField>::iterator it = m_fieldMap.find(strFieldName);
	if(it == m_fieldMap.end() )
	{
		return -1;
	}

	return it->second.iIndex;
}


Int32 CsvImpApr::getFieldType(int nFiledIndex)
{
	if(nFiledIndex >= (int)m_fieldData.size() )
	{
		return IRecordData::en_FieldType_Invalid;
	}

	Int8 cFieldType = m_fieldData[nFiledIndex].csvItemType;

	switch(cFieldType)
	{
		case CsvFile::en_CsvType_Int:
			return IRecordData::en_FieldType_Int;
			break;

		case CsvFile::en_CsvType_String:
			return IRecordData::en_FieldType_String;
			break;

		case CsvFile::en_CsvType_Float:
			return IRecordData::en_FieldType_Float;
			break;

		case CsvFile::en_CsvType_Macro:
			return IRecordData::en_FieldType_Macro;
			break;
	}

	return IRecordData::en_FieldType_Invalid;
}


std::string CsvImpApr::getString(Int32 row, Int32 nFiled, const char* strDefault)
{
	if(row >= getRecordCount() )
	{
		MINIAPR_ERROR("[%s]getString [%d]超过最大行数", m_strFileName.c_str(), row);
		return strDefault;
	}

	if(nFiled >= (int)m_fieldData.size())
	{
		MINIAPR_ERROR("[%s]getString [%d]超过最大列数", m_strFileName.c_str(), nFiled);
		return strDefault;
	}
	
	Int8 cFieldType = m_fieldData[nFiled].csvItemType;
	if( (cFieldType != CsvFile::en_CsvType_String) && (cFieldType != CsvFile::en_CsvType_Macro) )
	{
		MINIAPR_ERROR("[%s]getString 字段[%d]的类型是:%d", m_strFileName.c_str(), nFiled, m_fieldData[nFiled].csvItemType);
		return strDefault;
	}
	
	if(cFieldType == CsvFile::en_CsvType_String)
	{
		return m_csvData[row][nFiled].strItemData;
	}
	else if(cFieldType == CsvFile::en_CsvType_Macro)
	{
		return m_macroMap[m_csvData[row][nFiled].strItemData];
	}

	return strDefault;
}

Int32 CsvImpApr::getInt(Int32 row, Int32 nFiled, Int32 nDef)
{
	if(row >= getRecordCount() )
	{
		MINIAPR_ERROR("[%s]getInt [%d]超过最大行数", m_strFileName.c_str(), row);
		return nDef;
	}

	if(nFiled >= (int)m_fieldData.size())
	{
		MINIAPR_ERROR("[%s]getInt [%d]超过最大列数", m_strFileName.c_str(), nFiled);
		return nDef;
	}
	
	Int8 cFieldType = m_fieldData[nFiled].csvItemType;
	if( (cFieldType != CsvFile::en_CsvType_Int) && (cFieldType != CsvFile::en_CsvType_Macro) )
	{
		// en_CsvType_Null = 0,
		// en_CsvType_Int,
		// en_CsvType_String,
		// en_CsvType_Float,
		// en_CsvType_Macro,
		MINIAPR_ERROR("[%s]getInt 字段[%d]的类型是:%d", m_strFileName.c_str(), nFiled, m_fieldData[nFiled].csvItemType);
		return nDef;
	}

	// int 需要支持16进制
	string strValue;
	if(cFieldType == CsvFile::en_CsvType_Int)
	{
		strValue = m_csvData[row][nFiled].strItemData;
	}
	else if(cFieldType == CsvFile::en_CsvType_Macro)
	{
		strValue = m_macroMap[m_csvData[row][nFiled].strItemData].c_str();
	}
	else
	{
		return nDef;
	}

	char* pEndStr = NULL;
	double dValue = strtod(strValue.c_str(), &pEndStr);
	if(pEndStr == strValue.c_str() )
	{
		return nDef;
	}

	if(*pEndStr == 'X' || *pEndStr == 'x')
	{
		return (Int32)strtoul(strValue.c_str(), &pEndStr, 16);
	}

	return (Int32)((Uint32)dValue);
}

float CsvImpApr::getFloat(Int32 row, Int32 nField, float fDef)
{
	if(row >= getRecordCount() )
	{
		MINIAPR_ERROR("[%s]getFloat [%d]超过最大行数", m_strFileName.c_str(), row);
		return fDef;
	}

	if(nField >= (int)m_fieldData.size())
	{
		MINIAPR_ERROR("[%s]getFloat [%d]超过最大列数", m_strFileName.c_str(), nField);
		return fDef;
	}
	
	Int8 cFieldType = m_fieldData[nField].csvItemType;
	if((cFieldType != CsvFile::en_CsvType_Float) && (cFieldType != CsvFile::en_CsvType_Macro) )
	{
		MINIAPR_ERROR("[%s]getFloat 字段[%d]的类型是:%d",m_strFileName.c_str(), nField, m_fieldData[nField].csvItemType);
		return fDef;
	}

	if(cFieldType == CsvFile::en_CsvType_Float)
	{
		return (float)atof(m_csvData[row][nField].strItemData.c_str() );
	}
	else
	{
		return (float)atof(m_macroMap[m_csvData[row][nField].strItemData].c_str() );
	}
}

CsvImpApr::~CsvImpApr()
{
}

void CsvImpApr::setIndexField(const string& strFieldName)
{
    m_strIndexFieldName = strFieldName;
	Int32 nFieldIndex = -1;
	for(Int32 i = 0; i < (Int32)m_fieldData.size(); i++)
	{
		if(strFieldName == m_fieldData[i].strFieldName)
		{
			nFieldIndex = i;
			break;
		}
	}

	if( (nFieldIndex >= (Int32)m_fieldData.size() ) 
		|| (nFieldIndex < 0) )
	{
		MINIAPR_ERROR("[%s]设置索引失败，没有这个字段[%s]", m_strFileName.c_str(), strFieldName.c_str() );
		return;
	}

	for(Int32 i = 0; i < (Int32)m_csvData.size(); i++)
	{
		Int32 nIndexValue = getInt(i, nFieldIndex);
		m_indexMap[nIndexValue] = i;
	}
}

string CsvImpApr::getIndexFieldName()
{
    return m_strIndexFieldName;
}

Int32 CsvImpApr::findRecord(Int32 nIndexValue)
{
	MapIndex::iterator it = m_indexMap.find(nIndexValue);
	if(it != m_indexMap.end() )
	{
		return it->second;
	}

	return -1;
}

std::string CsvImpApr::getFieldName(Int32 nField)
{
	if( (nField >= (Int32)m_fieldData.size() ) || (nField < 0) )
	{
		return "";
	}

	return m_fieldData[nField].strFieldName;
}

void CsvImpApr::saveToDBC(const char* pszDbcFile, const char* strIndexName)
{
	ofstream ofs(pszDbcFile, ofstream::binary);
	if(!ofs.good() )
	{
		return;
	}

	Uint32 dwDbcFileFlag = 0x43424457;
	Uint32 dwRecordCount = (Uint32)m_csvData.size();
	Uint32 dwFieldCount = (Uint32)m_fieldData.size();
	Int32 nIndexFiled = -1;
	char* szFiledType = new char[dwFieldCount + 1];
	auto_ptr<char> holdFile(szFiledType);
	memset(szFiledType, 0, dwFieldCount);
	for(Int32 i = 0; i < (Int32)m_fieldData.size(); i++)
	{
		switch(m_fieldData[i].csvItemType)
		{
		case CsvFile::en_CsvType_Int:
			{
				szFiledType[i] = DbcFile::en_DbcType_Int;
				if(m_fieldData[i].strFieldName == strIndexName)
				{
					szFiledType[i] = DbcFile::en_DbcType_Index;
					nIndexFiled = i;
				}
			}
			break;
		case CsvFile::en_CsvType_Float:
			szFiledType[i] = DbcFile::en_DbcType_Float;
			break;
		case CsvFile::en_CsvType_Macro:
		case CsvFile::en_CsvType_String:
			szFiledType[i] = DbcFile::en_DbcType_String;
			break;
		}
	}
	
	
	std::map<string, Int32>	mapStringOffset;
	// 如果有索引，需要对行排序
	map<Int32, Int32>	rowList;
	for(Int32 i = 0; i < (Int32)dwRecordCount; i++)
	{
		if(nIndexFiled != -1)
		{
			Int32 nIndexValue = getInt(i, nIndexFiled, 0);
			rowList[nIndexValue] = i;
		}
		else
		{
			rowList[i] = i;
		}
	}

	Int32 m_recordSize = dwFieldCount*4;
	Int32 m_dataSize = (Int32)m_recordSize*dwRecordCount;
	char* pData = new char[m_dataSize];
	auto_ptr<char> holdData(pData);
	Int32 nCurrentOffset = 0;
	char* pWritePoint = pData;
	for(map<Int32, Int32>::iterator it = rowList.begin(); it != rowList.end(); it++)
	{
		Uint32 row = it->second;
		for(Uint32 col = 0; col < dwFieldCount; col++)
		{
			switch(szFiledType[col])
			{
			case DbcFile::en_DbcType_Int:
			case DbcFile::en_DbcType_Index:
				{
					*(Int32*)pWritePoint = atoi(m_csvData[row][col].strItemData.c_str() );
					pWritePoint += sizeof(Int32);
				}
				break;
			case DbcFile::en_DbcType_Float:
				{
					*(float*) pWritePoint = (float)atof(m_csvData[row][col].strItemData.c_str() );
					pWritePoint += sizeof(float);
				}
				break;
			case DbcFile::en_DbcType_String:
				{
					string strFieldData = m_csvData[row][col].strItemData;
					std::map<string, Int32>::iterator it = mapStringOffset.find(strFieldData);
					if(it != mapStringOffset.end() )
					{
						*(Int32*)pWritePoint = it->second;
						pWritePoint+= sizeof(Int32);
					}
					else
					{
						mapStringOffset[strFieldData] = nCurrentOffset;
						*(Int32*)pWritePoint = nCurrentOffset;
						pWritePoint+= sizeof(Int32);
						nCurrentOffset += (Int32)strFieldData.length() + 1;
					}
				}
				break;
			}
		}
	}
	
	Uint32 dwStringSize = (Uint32)nCurrentOffset;
	ofs.write( (const char*)&dwDbcFileFlag, sizeof(Uint32));
	ofs.write( (const char*)&dwRecordCount, sizeof(Uint32));
	ofs.write( (const char*)&dwFieldCount, sizeof(Uint32));
	ofs.write( (const char*)&m_recordSize, sizeof(Uint32));
	ofs.write( (const char*)&nCurrentOffset, sizeof(Uint32));

	// 写数据
	ofs.write( (const char*)pData, m_dataSize);

	long dwOffset = ofs.tellp();
	for(std::map<string, Int32>::iterator it = mapStringOffset.begin(); it != mapStringOffset.end(); it++)
	{
		ofs.seekp(dwOffset + it->second, ios_base::beg);
		ofs.write(it->first.c_str(), (std::streamsize)it->first.length() + 1);
	}

	// 写字段类型
	ofs.seekp(dwOffset + dwStringSize, ios_base::beg);
	ofs.write(szFiledType, dwFieldCount);

	// 写字段名称
	for(Int32 i = 0; i < getFieldCount(); i++)
	{
		ofs.write(m_fieldData[i].strFieldName.c_str(), (std::streamsize)m_fieldData[i].strFieldName.length() + 1);
	}
	ofs.close();
}
