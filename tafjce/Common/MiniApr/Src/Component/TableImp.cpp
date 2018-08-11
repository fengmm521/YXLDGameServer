#include "MiniAprPch.h"
#include "Component/IComponent.h"
#include "TableImp.h"

using namespace MINIAPR;
using namespace std;

CsvRecord::CsvRecord(CsvFile* pCsvFile, Int32 nRow):m_pCsvFile(pCsvFile), m_nRow(nRow)
{
}

CsvRecord::~CsvRecord()
{
}

Int32 CsvRecord::getInt(const std::string& strFieldName, Int32 nDef)
{
	int iIndex = m_pCsvFile->getFieldIndex(strFieldName);
	if(iIndex >= 0)
	{
		return m_pCsvFile->getInt(m_nRow, iIndex, nDef);
	}

	return nDef;
}

float CsvRecord::getFloat(const std::string& strFieldName, float fDef)
{
	int iIndex = m_pCsvFile->getFieldIndex(strFieldName);
	if(iIndex >= 0)
	{
		return m_pCsvFile->getFloat(m_nRow, iIndex, fDef);
	}

	return fDef;
}

std::string CsvRecord::getString(const std::string& strFieldName, const char* def)
{
	int iIndex = m_pCsvFile->getFieldIndex(strFieldName);
	if(iIndex >= 0)
	{
		return m_pCsvFile->getString(m_nRow, iIndex, def);
	}

	return def;
}

Int32 CsvRecord::getRawInt(Int32 nCol, Int32 nDef)
{
	return m_pCsvFile->getInt(m_nRow, nCol, nDef);
}

float CsvRecord::getRawFloat(Int32 nCol, float fDef)
{
	return m_pCsvFile->getFloat(m_nRow, nCol, fDef);
}

std::string CsvRecord::getRawString(Int32 nCol, const char* strDef)
{
	return m_pCsvFile->getString(m_nRow, nCol, strDef);
}


TableImp_Csv::TableImp_Csv(CsvFile* pFile, const std::string& strTableName):m_pCsvFile(pFile), m_strtableName(strTableName)
{
}

TableImp_Csv::~TableImp_Csv()
{
	delete m_pCsvFile;
}

std::string TableImp_Csv::getTableName()
{
	return m_strtableName;
}

Int32 TableImp_Csv::findRecord(Int32 nIndexValue)
{
	return m_pCsvFile->findRecord(nIndexValue);
}

Int32 TableImp_Csv::getInt(Int32 nRow, const std::string& strFieldName, Int32 nDef)
{
	CsvRecord record(m_pCsvFile, nRow);
	Int32 nRet = record.getInt(strFieldName, nDef);

	return nRet;
}

float TableImp_Csv::getFloat(Int32 nRow, const std::string& strFieldName, float fDef)
{
	CsvRecord record(m_pCsvFile, nRow);
	float fRet = record.getFloat(strFieldName, fDef);

	return fRet;
}

std::string TableImp_Csv::getString(Int32 nRow, const std::string& strFieldName, const char* strDef)
{
	CsvRecord record(m_pCsvFile, nRow);
	std::string strRet = record.getString(strFieldName, strDef);

	return strRet;
}

Int32 TableImp_Csv::getRawInt(Int32 nRow, Int32 nCol, Int32 nDef)
{
	CsvRecord record(m_pCsvFile, nRow);
	Int32 nRet = record.getRawInt(nCol, nDef);

	return nRet;
}

float TableImp_Csv::getRawFloat(Int32 nRow, Int32 nCol, float fDef)
{
	CsvRecord record(m_pCsvFile, nRow);
	float fRet = record.getRawFloat(nCol, fDef);

	return fRet;
}

std::string TableImp_Csv::getRawString(Int32 nRow, Int32 nCol, const char* strDef)
{
	CsvRecord record(m_pCsvFile, nRow);
	std::string strRet = record.getRawString(nCol, strDef);

	return strRet;
}

IRecordData::Handle TableImp_Csv::getRecord(Int32 nRow)
{
	CsvRecord* pCsvRecord = new CsvRecord(m_pCsvFile, nRow);

	IRecordData::Handle handle;
	handle.bind(pCsvRecord);

	return handle;
}

Int32 TableImp_Csv::getRecordCount()
{
	return m_pCsvFile->getRecordCount();
}

Int32 TableImp_Csv::getFieldCount()
{
	return m_pCsvFile->getFieldCount();
}

std::string TableImp_Csv::getFieldName(int nFileIndex)
{
	std::string strFiledName = m_pCsvFile->getFieldName(nFileIndex);
	return strFiledName;
}

Int32 TableImp_Csv::getFieldType(Int32 nFiledIndex)
{
	Int32 nFiledType = m_pCsvFile->getFieldType(nFiledIndex);

	return nFiledType;
}

bool TableImp_Csv::initPropSet(IPropDefine* pPropDef, int nRow, PropertySet& propSet, bool bErrLog)
{
	if(!pPropDef || (nRow < 0) )
	{
		return false;
	}

	if(nRow >= getRecordCount() )
	{
		return false;
	}

	int nFiledCount = getFieldCount();
	for(int i = 0; i < nFiledCount; i++)
	{
		string strFieldName = getFieldName(i);
		int nFieldType = getFieldType(i);
		int nPropId = pPropDef->getPropKey(strFieldName);
		if(nPropId == -1)
		{
			if(bErrLog)
			{
				MINIAPR_ERROR("table[%s] 不存在属性:[%s]", m_strtableName.c_str(), strFieldName.c_str() );
			}
			continue;
		}

		switch(nFieldType)
		{
			case IRecordData::en_FieldType_Int:
				propSet.setInt(nPropId, (Int32)getInt(nRow, strFieldName) );
				break;

			case IRecordData::en_FieldType_String:
				propSet.setString(nPropId, getString(nRow, strFieldName).c_str() );
				break;

			case IRecordData::en_FieldType_Float:
				propSet.setFloat(nPropId, getFloat(nRow, strFieldName) );
				break;
		}
	}

	return true;
}

bool TableImp_Csv::reload()
{
    if(!m_pCsvFile)
        return false;
    return m_pCsvFile->reload();
}

//返回指针需要调用方自行释放
ITable* TableImp_Csv::cloneTableFromStorage()
{
    if(!m_pCsvFile)
        return NULL;
    CsvFile* pFile = m_pCsvFile->cloneCsvFromStorage();
    if(pFile)
        return new TableImp_Csv(pFile, m_strtableName);
    return NULL;
}

DbcRecord::DbcRecord(DbcFile* pDbcFile, Int32 nRow):m_pDbcFile(pDbcFile), m_nRow(nRow)
{
}

DbcRecord::~DbcRecord()
{
}

Int32 DbcRecord::getInt(const std::string& strFieldName, Int32 nDef)
{
	for(Int32 i = 0; i < m_pDbcFile->getFieldCount(); i++)
	{
		if(m_pDbcFile->getFieldName(i) == strFieldName)
		{
			return m_pDbcFile->getInt(m_nRow, i, nDef);
		}
	}

	return nDef;
}


float DbcRecord::getFloat(const std::string& strFieldName, float fDef)
{
	for(Int32 i = 0; i < m_pDbcFile->getFieldCount(); i++)
	{
		if(m_pDbcFile->getFieldName(i) == strFieldName)
		{
			return m_pDbcFile->getFloat(m_nRow, i, fDef);
		}
	}

	return fDef;
}

std::string DbcRecord::getString(const std::string& strFieldName, const char* def)
{
	for(Int32 i = 0; i < m_pDbcFile->getFieldCount(); i++)
	{
		if(m_pDbcFile->getFieldName(i) == strFieldName)
		{
			return m_pDbcFile->getString(m_nRow, i, (char*)def);
		}
	}

	return def;
}

Int32 DbcRecord::getRawInt(Int32 nCol, Int32 nDef)
{
	return m_pDbcFile->getInt(m_nRow, nCol, nDef);
}

float DbcRecord::getRawFloat(Int32 nCol, float fDef)
{
	return m_pDbcFile->getFloat(m_nRow, nCol, fDef);
}

std::string DbcRecord::getRawString(Int32 nCol, const char* strDef)
{
	return m_pDbcFile->getString(m_nRow, nCol, (char*)strDef);
}

TableImp_Dbc::TableImp_Dbc(DbcFile* pDbcFile, const std::string& strTableName):m_pDbcFile(pDbcFile), m_strTableName(strTableName)
{
}

TableImp_Dbc::~TableImp_Dbc()
{
	delete m_pDbcFile;
}

std::string TableImp_Dbc::getTableName()
{
	return m_strTableName;
}

Int32 TableImp_Dbc::findRecord(Int32 nIndexValue)
{
	return m_pDbcFile->findByIndex(nIndexValue);
}

IRecordData::Handle TableImp_Dbc::getRecord(Int32 nRow)
{
	DbcRecord* pDbcRecord = new DbcRecord(m_pDbcFile, nRow);

	IRecordData::Handle handle;
	handle.bind(pDbcRecord);

	return handle;
}

Int32 TableImp_Dbc::getRecordCount()
{
	return m_pDbcFile->getRecordCount();
}

Int32 TableImp_Dbc::getFieldCount()
{
	return m_pDbcFile->getFieldCount();
}

std::string TableImp_Dbc::getFieldName(int nFileIndex)
{
	std::string strFiledName = m_pDbcFile->getFieldName(nFileIndex);
	return strFiledName;
}

Int32 TableImp_Dbc::getInt(Int32 nRow, const std::string& strFieldName, Int32 nDef)
{
	DbcRecord record(m_pDbcFile, nRow);
	Int32 nRet = record.getInt(strFieldName, nDef);

	return nRet;
}

float TableImp_Dbc::getFloat(Int32 nRow, const std::string& strFieldName, float fDef)
{
	DbcRecord record(m_pDbcFile, nRow);
	float fRet = record.getFloat(strFieldName, fDef);

	return fRet;
}

std::string TableImp_Dbc::getString(Int32 nRow, const std::string& strFieldName, const char* strDef)
{
	DbcRecord record(m_pDbcFile, nRow);
	std::string strRet = record.getString(strFieldName, strDef);

	return strRet;
}

Int32 TableImp_Dbc::getRawInt(Int32 nRow, Int32 nCol, Int32 nDef)
{
	DbcRecord record(m_pDbcFile, nRow);
	Int32 nRet = record.getRawInt(nCol, nDef);

	return nRet;
}

float TableImp_Dbc::getRawFloat(Int32 nRow, Int32 nCol, float fDef)
{
	DbcRecord record(m_pDbcFile, nRow);
	float fRet = record.getRawFloat(nCol, fDef);

	return fRet;
}

std::string TableImp_Dbc::getRawString(Int32 nRow, Int32 nCol, const char* strDef)
{
	DbcRecord record(m_pDbcFile, nRow);
	std::string strRet = record.getRawString(nCol, strDef);

	return strRet;
}

Int32 TableImp_Dbc::getFieldType(Int32 nFiledIndex)
{
	Int32 nFiledType = m_pDbcFile->getFieldType(nFiledIndex);

	return nFiledType;
}


PropDefineImp::PropDefineImp(const std::string& strCsvFile):m_strCsvFile(strCsvFile)
{
	loadPropDefData(strCsvFile);
}

PropDefineImp::~PropDefineImp()
{
}

void PropDefineImp::loadPropDefData(const std::string& strCsvFile)
{
	CsvFile tmpFile;
	if(!tmpFile.open(m_strCsvFile.c_str() ) )
	{
		return;
	}

	int nRowCount = tmpFile.getRecordCount();
	for(int i = 0; i < nRowCount; i++)
	{
		string strKeyName = tmpFile.getString(i, 0, "");
		int nValue = tmpFile.getInt(i, 1, 0);
		string strKeyType = tmpFile.getString(i, 2, "");
		
		KeyInfo tmpInfo;
		tmpInfo.strKeyName = strKeyName;
		tmpInfo.nKeyValue = nValue;
		tmpInfo.strKeyType = strKeyType;
		m_keyMap[strKeyName] = tmpInfo;
		m_keyIntMap[nValue] = tmpInfo;
	}
}

string PropDefineImp::getPropName(int iKey)
{
	std::map<int, KeyInfo>::iterator it = m_keyIntMap.find(iKey);
	if(it == m_keyIntMap.end() )
	{
		return "";
	}

	KeyInfo& info = it->second;
	return info.strKeyName;
}

int PropDefineImp::getPropKey(const std::string& strKeyName, int nDef)
{
	std::map<std::string, KeyInfo>::iterator it = m_keyMap.find(strKeyName);
	if(it == m_keyMap.end() )
	{
		return nDef;
	}

	KeyInfo& info = it->second;
	return info.nKeyValue;
}

std::string PropDefineImp::getPropType(const std::string& strKeyName)
{
	std::map<std::string, KeyInfo>::iterator it = m_keyMap.find(strKeyName);
	if(it == m_keyMap.end() )
	{
		return "";
	}

	KeyInfo& info = it->second;
	return info.strKeyType;
}


IPropDefine* PropDefineImp::clone() const
{
	return new PropDefineImp(m_strCsvFile);
}



