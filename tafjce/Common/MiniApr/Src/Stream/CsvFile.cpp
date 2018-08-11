#include "MiniAprPch.h"
#include "Stream/CsvFile.h"
#include "CsvImp.h"

using namespace MINIAPR;
using namespace std;


CsvFile::CsvFile():m_pCsvImp(new CsvImpApr() )
{
}

CsvFile::~CsvFile()
{
	if(m_pCsvImp)
	{
		delete m_pCsvImp;
	}
}

bool CsvFile::open(const char* strFileName)
{
	if(!m_pCsvImp)
	{
		return false;
	}

	return m_pCsvImp->open(strFileName);
}

bool CsvFile::reload()
{
	if(!m_pCsvImp)
	{
		return false;
	}
    
    string strFileName = m_pCsvImp->getFileName();
    string strIndexFiledName = m_pCsvImp->getIndexFieldName();
    if(strFileName.size() == 0)
    {
        return false;
    }
    
    CsvImpInterface* pNewCsvImp = new CsvImpApr();
    if(pNewCsvImp->open(strFileName.c_str()))
    {
        if(!strIndexFiledName.empty())
            pNewCsvImp->setIndexField(strIndexFiledName);
            
        delete m_pCsvImp;
        m_pCsvImp = pNewCsvImp;
        return true;
    }
    else
    {
        delete pNewCsvImp;
    }
    return false;
}

CsvFile* CsvFile::cloneCsvFromStorage()
{
	if(!m_pCsvImp)
	{
		return NULL;
	}
    string strFileName = m_pCsvImp->getFileName();
    string strIndexFiledName = m_pCsvImp->getIndexFieldName();
    if(strFileName.size() == 0)
    {
        return NULL;
    }
    CsvFile* pCloneCsvFile = new CsvFile();
    if(!pCloneCsvFile)
        return NULL;
    if(!pCloneCsvFile->open(strFileName.c_str()))
    {
        delete pCloneCsvFile;
        pCloneCsvFile = NULL;
        return NULL;
    }
    if(!strIndexFiledName.empty())
        pCloneCsvFile->setIndexField(strIndexFiledName);
    return pCloneCsvFile;
}

Int32 CsvFile::getRecordCount()
{
	if(!m_pCsvImp)
	{
		return 0;
	}

	return m_pCsvImp->getRecordCount();
}

Int32 CsvFile::getFieldCount()
{
	if(!m_pCsvImp)
	{
		return 0;
	}

	return m_pCsvImp->getFieldCount();
}

std::string CsvFile::getString(Int32 row, Int32 nFiled, const char* strDefault)
{
	if(!m_pCsvImp)
	{
		return strDefault;
	}

	return m_pCsvImp->getString(row, nFiled, strDefault);
}

Int32 CsvFile::getInt(Int32 row, Int32 nField, Int32 nDef)
{
	if(!m_pCsvImp)
	{
		return nDef;
	}

	return m_pCsvImp->getInt(row, nField, nDef);
}

float CsvFile::getFloat(Int32 row, Int32 nField, float fDef)
{
	if(!m_pCsvImp)
	{
		return fDef;
	}

	return m_pCsvImp->getFloat(row, nField, fDef);
}

void CsvFile::saveToDBC(const char* strDbcFile, const char* strIndexName)
{
	if(!strDbcFile)
	{
		return;
	}
	
	return m_pCsvImp->saveToDBC(strDbcFile, strIndexName);
}

std::string CsvFile::getFieldName(Int32 nField)
{
	return m_pCsvImp->getFieldName(nField);
}

Int32 CsvFile::getFieldType(int nFiledIndex)
{
	return m_pCsvImp->getFieldType(nFiledIndex);
}


void CsvFile::setIndexField(const string& strFieldName)
{
	m_pCsvImp->setIndexField(strFieldName);
}

Int32 CsvFile::findRecord(Int32 nIndexValue)
{
	return m_pCsvImp->findRecord(nIndexValue);
}

int CsvFile::getFieldIndex(const std::string& strFieldName)
{
	return m_pCsvImp->getFieldIndex(strFieldName);
}

