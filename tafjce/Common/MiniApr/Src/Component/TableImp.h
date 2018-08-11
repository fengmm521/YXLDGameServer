#ifndef __TABLE_IMP_H__
#define __TABLE_IMP_H__

#include "Stream/CsvFile.h"
#include "Stream/DbcFile.h"

BEGIN_MINIAPR_NAMESPACE

class CsvRecord:public IRecordData
{
public:
	CsvRecord(CsvFile* pCsvFile, Int32 nRow);
	~CsvRecord();

	virtual Int32 getInt(const std::string& strFieldName, Int32 nDef = 0);
	virtual float getFloat(const std::string& strFieldName, float fDef = 0.0f);
	virtual std::string getString(const std::string& strFieldName, const char* ="");
	virtual Int32 getRawInt(Int32 nCol, Int32 nDef = 0);
	virtual float getRawFloat(Int32 nCol, float fDef = 0.0f);
	virtual std::string getRawString(Int32 nCol, const char* strDef = "");
private:
	CsvFile*	m_pCsvFile;
	Int32		m_nRow;
};


class TableImp_Csv:public ITable
{
public:
	
	TableImp_Csv(CsvFile* pFile, const std::string& strTableName);
	~TableImp_Csv();
	virtual std::string getTableName();
	virtual Int32 findRecord(Int32 nIndexValue);
	virtual IRecordData::Handle getRecord(Int32 nRow);
	virtual Int32 getRecordCount();
	virtual Int32 getFieldCount();
	virtual std::string getFieldName(int nFileIndex);

	virtual Int32 getInt(Int32 nRow, const std::string& strFieldName, Int32 nDef = 0);
	virtual float getFloat(Int32 nRow, const std::string& strFieldName, float fDef = 0.0f);
	virtual std::string getString(Int32 nRow, const std::string& strFieldName, const char* strDef ="");
	virtual Int32 getRawInt(Int32 nRow, Int32 nCol, Int32 nDef = 0);
	virtual float getRawFloat(Int32 nRow, Int32 nCol, float fDef = 0.0f);
	virtual std::string getRawString(Int32 nRow, Int32 nCol, const char* strDef = "");
	virtual Int32 getFieldType(Int32 nFiledIndex);
	virtual bool initPropSet(IPropDefine* pPropDef, int nRow, PropertySet& propSet, bool bErrLog);
    
    virtual bool reload();
    //返回指针需要调用方自行释放
    virtual ITable* cloneTableFromStorage();

private:
	CsvFile*	m_pCsvFile;
	std::string m_strtableName;
};


class DbcRecord:public IRecordData
{
public:

	DbcRecord(DbcFile* pDbcFile, Int32 nRow);
	~DbcRecord();

	virtual Int32 getInt(const std::string& strFieldName, Int32 nDef = 0);
	virtual float getFloat(const std::string& strFieldName, float fDef = 0.0f);
	virtual std::string getString(const std::string& strFieldName, const char* ="");
	virtual Int32 getRawInt(Int32 nCol, Int32 nDef = 0);
	virtual float getRawFloat(Int32 nCol, float fDef = 0.0f);
	virtual std::string getRawString(Int32 nCol, const char* strDef = "");

private:
	DbcFile*	m_pDbcFile;
	Int32		m_nRow;
};


class TableImp_Dbc:public ITable
{
public:
	
	TableImp_Dbc(DbcFile* pDbcFile, const std::string& strTableName);
	~TableImp_Dbc();
	virtual std::string getTableName();
	virtual Int32 findRecord(Int32 nIndexValue);
	virtual IRecordData::Handle getRecord(Int32 nRow);
	virtual Int32 getRecordCount();
	virtual Int32 getFieldCount();
	virtual std::string getFieldName(int nFileIndex);

	virtual Int32 getInt(Int32 nRow, const std::string& strFieldName, Int32 nDef = 0);
	virtual float getFloat(Int32 nRow, const std::string& strFieldName, float fDef = 0.0f);
	virtual std::string getString(Int32 nRow, const std::string& strFieldName, const char* strDef ="");
	virtual Int32 getRawInt(Int32 nRow, Int32 nCol, Int32 nDef = 0);
	virtual float getRawFloat(Int32 nRow, Int32 nCol, float fDef = 0.0f);
	virtual std::string getRawString(Int32 nRow, Int32 nCol, const char* strDef = "");
	virtual Int32 getFieldType(Int32 nFiledIndex);
	virtual bool initPropSet(IPropDefine* pPropDef, int nRow, PropertySet& propSet, bool bErrLog){assert(false);}
    virtual bool reload() { return false;};
    virtual ITable* cloneTableFromStorage() { return NULL;};
private:
	
	DbcFile*	m_pDbcFile;
	std::string m_strTableName;
};


class PropDefineImp:public IPropDefine
{
public:

	struct KeyInfo
	{
		std::string strKeyName;
		int nKeyValue;
		std::string strKeyType;
	};

	PropDefineImp(const std::string& strCsvFile);
	~PropDefineImp();

	
	virtual int getPropKey(const std::string& strKeyName, int nDef = -1);
	virtual std::string getPropType(const std::string& strKeyName);
	virtual IPropDefine* clone() const;
	virtual string getPropName(int iKey);

private:

	void loadPropDefData(const std::string& strCsvFile);
	
private:
	std::string m_strCsvFile;
	std::map<std::string, KeyInfo>	m_keyMap;
	std::map<int, KeyInfo> m_keyIntMap;
};


END_MINIAPR_NAMESPACE

#endif
