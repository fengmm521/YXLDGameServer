#include "MiniAprPch.h"
#include "Stream/DbcFile.h"

using namespace MINIAPR;
using namespace std;

#define DBC_MAGC	0x43424457

#define CHECK_OUTOFRAGE()\
if(nRow >= (Int32)m_recordCount)\
{\
	/*APR_ERROR("ROW:%d超过最大行:%d", nRow, m_recordCount);*/\
	return Def;\
}\
if(nField >= (Int32)m_fieldCount)\
{\
	return Def;\
}

DbcFile::DbcFile():m_recordSize(0),m_recordCount(0), m_fieldCount(0), m_stringSize(0),
m_fieldsOffset(NULL), m_szFieldType(NULL), m_data(NULL), m_stringTable(NULL)
{
}

DbcFile::~DbcFile()
{
	if(m_fieldsOffset)
	{
		delete []m_fieldsOffset;
	}

	if(m_data)
	{
		delete []m_data;
	}

	if(m_szFieldType)
	{
		delete []m_szFieldType;
	}
}

bool DbcFile::open(const std::string& strFileName, bool useInnerFiled)
{
	m_strFileName = strFileName;
	Uint32 header;
	if(m_data)
	{
		delete [] m_data;
		m_data=NULL;
	}
	
	ifstream ifs(strFileName.c_str(), ifstream::binary);
	if(!ifs.good() ) return false;

	ifs.read( (char*)&header, sizeof(header) );
	if(DBC_MAGC != header)
	{
		//APR_ERROR("%s 不是一个合法的DBC文件");
		return false;
	}
	
	ifs.read( (char*)&m_recordCount, sizeof(m_recordCount) );
	ifs.read( (char*)&m_fieldCount, sizeof(m_fieldCount) );
	ifs.read( (char*)&m_recordSize, sizeof(m_recordSize) );
	ifs.read( (char*)&m_stringSize, sizeof(m_stringSize) );

	m_data = new unsigned char[m_recordSize*m_recordCount+m_stringSize];
	m_stringTable = m_data + m_recordSize*m_recordCount;
	ifs.read( (char*)m_data, m_recordSize*m_recordCount + m_stringSize);

	// 扩展，读取各个字段属性
	if(useInnerFiled)
	{
		char* pszFileType = new char[m_fieldCount + 1];
		ifs.read(pszFileType, m_fieldCount);
		pszFileType[m_fieldCount] = '\0';
		setFileType(pszFileType);
		delete []pszFileType;

		// 读取各个字段名称
		for(Uint32 i = 0; i < m_fieldCount; i++)
		{
			char szFieldName[4096] = {0};
			for(Int32 rdIndex = 0;rdIndex < (Int32)sizeof(szFieldName);rdIndex++)
			{
				Int8 ch = (Int8)ifs.get();
				if( (ch == '\0') || !ifs.good() )
				{
					szFieldName[rdIndex] = '\0';
					break;
				}
				szFieldName[rdIndex] = ch;
			}
			szFieldName[sizeof(szFieldName) - 1] = '\0';
			m_fieldNameList.push_back(szFieldName);
		}
	}

	ifs.close();
	return true;
}

std::string DbcFile::getFieldName(Int32 nField)
{
	if( (nField < 0) || (nField >= (Int32)m_fieldNameList.size() ) )
	{
		return "";
	}

	return m_fieldNameList[nField];
}

Int32 DbcFile::findByIndex(Int32 nIndexValue)
{
	Uint32 nOffset = 0;
	bool bFindIndex = false;
	for(Uint32 i = 0; i < m_fieldCount; i++)
	{
		if(m_szFieldType[i] == en_DbcType_Index)
		{
			nOffset = m_fieldsOffset[i];
			bFindIndex = true;
			break;
		}
	}

	if(!bFindIndex)
	{
		//APR_ERROR("%s没有定义索引项", m_strFileName.c_str() );
		return -1;
	}

	Int32 nLeftRow = 0;
	Int32 nRightRow = m_recordCount - 1;

	for(;;)
	{
		Int32 nMidIndex = nLeftRow + (nRightRow - nLeftRow)/2;
		Int32 nMidValue = *(Int32*)(m_data + nMidIndex * m_recordSize + nOffset);
		
		if(nIndexValue == nMidValue)
		{
			return nMidIndex;
		}

		if(nLeftRow >= nRightRow)
		{
			return -1;
		}

		if(nIndexValue < nMidValue)
		{
			//nLeftRow++;
			nRightRow = --nMidIndex;
		}
		else
		{
			nLeftRow = ++nMidIndex;
		}
	}
}

void DbcFile::save(const std::string strFileName)
{
	ofstream ofs(strFileName.c_str(), ofstream::binary);
	if(!ofs.good() ) return;

	Uint32 dwHeader = 0x43424457;
	ofs.write( (char*)&dwHeader, sizeof(dwHeader) );

	ofs.write( (char*)&m_recordCount, sizeof(m_recordCount) );
	ofs.write( (char*)&m_fieldCount, sizeof(m_fieldCount) );
	ofs.write( (char*)&m_recordSize, sizeof(m_recordSize) );
	ofs.write( (char*)&m_stringSize, sizeof(m_stringSize) );

	ofs.write( (char*)m_data, m_recordSize*m_recordCount+m_stringSize);

	// 写类型字段
	ofs.write( (char*)m_szFieldType, m_fieldCount);
	ofs.close();
}

Int32 DbcFile::getRecordCount()
{
	return m_recordCount;
}

Int32 DbcFile::getFieldCount()
{
	return m_fieldCount;
}

std::string DbcFile::getString(Int32 nRow, Int32 nField, char* Def)
{
	CHECK_OUTOFRAGE();
	if(m_szFieldType[nField] != en_DbcType_String)
	{
		//APR_ERROR("字段类型错误:%d不是string", m_szFieldType[nField]);
		return Def;
	}

	Int32 nOffset = *(Int32*)(m_data + nRow*m_recordSize + m_fieldsOffset[nField]);
	if(nOffset >= (Int32)m_stringSize)
	{
		//APR_ERROR("getString失败，超过字符串偏移:%d", nOffset);
		return Def;
	}

	return (char*)(m_stringTable + nOffset);
}

Int32 DbcFile::getInt(Int32 nRow, Int32 nField, Int32 Def)
{
	CHECK_OUTOFRAGE();

	if( (m_szFieldType[nField] != en_DbcType_Int) && (m_szFieldType[nField] != en_DbcType_Byte)&& (m_szFieldType[nField] != en_DbcType_Bool)&&(m_szFieldType[nField] != en_DbcType_Index) )
	{
		return Def;
	}

	Uint8* pRowData = m_data + nRow*m_recordSize;
	pRowData += m_fieldsOffset[nField];

	Int32 nRetCode = Def;
	switch(m_szFieldType[nField])
	{
	case en_DbcType_Index:
	case en_DbcType_Int:
		nRetCode = *(Int32*)pRowData;
		break;
	case en_DbcType_Byte:
	case en_DbcType_Bool:
		nRetCode = *(Uint8*)pRowData;
		break;
	}
	
	return nRetCode;
}

Int32 DbcFile::getFieldType(int nFiledIndex)
{
	if(nFiledIndex >= (Int32)m_fieldCount)
	{
		return IRecordData::en_FieldType_Invalid;
	}

	switch(m_szFieldType[nFiledIndex])
	{
	case en_DbcType_Index:
	case en_DbcType_Int:
		return IRecordData::en_FieldType_Int;
		break;
		
	case en_DbcType_Byte:
	case en_DbcType_Bool:
		return IRecordData::en_FieldType_Int;
		break;

	case en_DbcType_Float:
		return IRecordData::en_FieldType_Float;
		break;

	case en_DbcType_String:
		return IRecordData::en_FieldType_String;
		break;
	}

	return IRecordData::en_FieldType_Invalid;
}

bool DbcFile::getBool(Int32 nRow, Int32 nField, bool Def)
{
	CHECK_OUTOFRAGE();
	if(m_szFieldType[nField] != en_DbcType_Bool)
	{
		return Def;
	}

	Uint8* pRowData = m_data + nRow*m_recordSize;
	pRowData += m_fieldsOffset[nField];
	return (*pRowData) != 0;
}

float DbcFile::getFloat(Int32 nRow, Int32 nField, float Def)
{
	CHECK_OUTOFRAGE();
	
	if(m_szFieldType[nField] != en_DbcType_Float)
	{
		return Def;
	}

	Uint8* pRowData = m_data + nRow*m_recordSize;
	pRowData += m_fieldsOffset[nField];

	return *(float*)pRowData;
}

void DbcFile::saveToCsv(const std::string strFileName)
{
	if(NULL == m_szFieldType)
	{
		//APR_ERROR("DBC：%s保存为CSV失败,没有设置DBC各列类型", m_strFileName.c_str() );
		return;
	}

	ofstream ofs(strFileName.c_str(), ofstream::binary);
	if(!ofs.good() ) return;

	// 如果有各个列名字，写名字
	if(m_fieldNameList.size() == m_fieldCount)
	{
		for(Int32 i = 0; i < (Int32)m_fieldNameList.size(); i++)
		{
			if(i != 0) ofs.write(",", 1);
			ofs.write(m_fieldNameList[i].c_str(), (std::streamsize)m_fieldNameList[i].length() );
		}
		ofs.write("\n", 1);
	}

	// 写各个列的类型
	for(Int32 i = 0; i < (Int32)m_fieldCount; i++)
	{
		string strTypeName;
		switch(m_szFieldType[i])
		{
		case en_DbcType_Bool:
		case en_DbcType_Int:
		case en_DbcType_Index:
		case en_DbcType_Byte:
			strTypeName = "int";
			break;
		case en_DbcType_Float:
			strTypeName = "float";
			break;
		case en_DbcType_String:
			strTypeName = "string";
			break;
		default:
			strTypeName = "int";
		}

		if(i != 0) ofs.write(",", 1);
		ofs.write(strTypeName.c_str(), (std::streamsize)strTypeName.length() );
	}
	
	ofs.write("\n", 1);

	// 开始每行数据了
	for(Int32 row = 0; row < getRecordCount(); row++)
	{
		for(Int32 col = 0; col < getFieldCount(); col++)
		{
			string strItemData;
			switch(m_szFieldType[col])
			{
			case en_DbcType_Bool:
			case en_DbcType_Int:
			case en_DbcType_Index:
			case en_DbcType_Byte:
				{
					char szValue[32] = {0};
					sprintf(szValue, "%d", getInt(row, col) );
					strItemData = szValue;
				}
				break;
			case en_DbcType_Float:
				{
					char szValue[32] = {0};
					sprintf(szValue, "%f", getFloat(row, col) );
					strItemData = szValue;
				}
				break;
			case en_DbcType_String:
				{
					strItemData = getString(row, col);
					size_t sPos = 0;
					while( (sPos = strItemData.find(",") ) != string::npos)
					{
						strItemData.at(sPos) = ' ';
					}
				}
				break;
			}

			if(col != 0) ofs.write(",", 1);
			ofs.write(strItemData.c_str(), (std::streamsize)strItemData.length() );
		}
		ofs.write("\n", 1);
	}
	ofs.close();
}

void DbcFile::setFileType(const std::string fieldFmt)
{
	if(fieldFmt.length() != m_fieldCount)
	{
		//APR_ERROR("设置DBC文件格式错误:%d!=%d", fieldFmt.length(), m_fieldCount);
		return;
	}

	if(m_szFieldType)
	{
		delete []m_szFieldType;
		m_szFieldType = NULL;
	}
	m_szFieldType = new Uint8[m_fieldCount];
	memset(m_szFieldType, 0,m_fieldCount);
	memcpy(m_szFieldType, fieldFmt.c_str(), fieldFmt.length() );
	
	if(m_fieldsOffset)
	{
		delete []m_fieldsOffset;
		m_fieldsOffset = NULL;
	}

	m_fieldsOffset = new Uint32[m_fieldCount];
	m_fieldsOffset[0] = 0;
	for(Uint32 i = 1; i < m_fieldCount; i++)
	{
		m_fieldsOffset[i] = m_fieldsOffset[i - 1];
		if (m_szFieldType[i - 1] == 'b' || m_szFieldType[i - 1] == 'X')
		{
			m_fieldsOffset[i] += 1;
		}
		else
		{
			m_fieldsOffset[i] += 4;
		}
	}
}
