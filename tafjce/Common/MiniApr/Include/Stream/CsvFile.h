#ifndef __CSV_FILE_H__
#define __CSV_FILE_H__

BEGIN_MINIAPR_NAMESPACE

class CsvImpInterface
{
public:

	virtual bool open(const char* strFileName) = 0;
	virtual string getFileName() = 0;
	virtual Int32 getRecordCount() = 0;
	virtual Int32 getFieldCount() = 0;
	virtual std::string getString(Int32 row, Int32 nFiled, const char* strDefault="") = 0;
	virtual Int32 getInt(Int32 row, Int32 nField, Int32 nDef = 0) = 0;
	virtual float getFloat(Int32 row, Int32 nField, float fDef = 0.0) = 0;
	virtual void saveToDBC(const char* pszDbcFile, const char* strIndexName) = 0;
	virtual void setIndexField(const std::string& strFieldName) = 0;
	virtual Int32 findRecord(Int32 nIndexValue) = 0;
	virtual std::string getFieldName(Int32 nField) = 0;
	virtual Int32 getFieldType(int nFiledIndex) = 0;
	virtual Int32 getFieldIndex(const std::string& strFieldName) = 0;
	virtual string getIndexFieldName() = 0;

	virtual ~CsvImpInterface(){}
};

class  CsvFile
{
public:

	enum
	{
		en_CsvType_Null = 0,
		en_CsvType_Int,
		en_CsvType_String,
		en_CsvType_Float,
		en_CsvType_Macro,
	};
	
	CsvFile();
	~CsvFile();
	
/*!
\brief  
	打开csv文件
\param[in]const char* strFileName
	文件名路径
\return bool 
	true 成功
	false 失败
*/
	bool open(const char* strFileName);
	
	Int32 getRecordCount();
	
	Int32 getFieldCount();

	std::string getString(Int32 row, Int32 nFiled, const char* strDefault="");

	Int32 getInt(Int32 row, Int32 nField, Int32 nDef = 0);

	float getFloat(Int32 row, Int32 nField, float fDef = 0.0);

	void saveToDBC(const char* strDbcFile, const char* strIndexName);

	void setIndexField(const std::string& strFieldName);

	Int32 findRecord(Int32 nIndexValue);

	std::string getFieldName(Int32 nField);

	Int32 getFieldType(int nFiledIndex);

	int getFieldIndex(const std::string& strFieldName);

    //重新加载之前己成功加载的table
    bool reload();
    //返回指针需要调用方自行释放
    CsvFile* cloneCsvFromStorage();

private:
	
	CsvImpInterface*	m_pCsvImp;
};

END_MINIAPR_NAMESPACE

#endif
