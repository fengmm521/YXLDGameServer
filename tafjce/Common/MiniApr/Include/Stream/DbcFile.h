#ifndef __DBC_FILE_H__
#define __DBC_FILE_H__

BEGIN_MINIAPR_NAMESPACE

#ifdef WIN32
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

class  DbcFile
{
public:
	enum
	{
		en_DbcType_Na='x',                                              //not used or unknown, 4 byte size
		en_DbcType_NaByte='X',                                         //not used or unknown, byte
		en_DbcType_String='s',                                          //char*
		en_DbcType_Float='f',                                           //float
		en_DbcType_Int='i',                                             //uint32
		en_DbcType_Byte='b',                                            //uint8
		en_DbcType_Sort='d',                                            //sorted by this field, field is not included
		en_DbcType_Index='n',                                             //the same,but parsed to data
		en_DbcType_Bool='l',                                            //Logical (boolean)
	};
	
	DbcFile();

	~DbcFile();

	bool open(const std::string& strFileName,bool useInnerFiled = false);
	
	Int32 getRecordCount();

	Int32 getFieldCount();
	
	std::string getString(Int32 nRow, Int32 nField, char* strDef = "");
	
	Int32 getInt(Int32 nRow, Int32 nField, Int32 nDef=0);

	bool getBool(Int32 nRow, Int32 nField, bool bDef=false);

	float getFloat(Int32 nRow, Int32 nField, float fDef = 0.0f);

	void saveToCsv(const std::string strFileName);

	void save(const std::string strFileName);

	void setFileType(const std::string fieldFmt);

	Int32 findByIndex(Int32 nIndexValue);

	Int32 getFieldType(int nFiledIndex);

	std::string getFieldName(Int32 nField);
private:
	
	Uint32	m_recordSize;
	Uint32	m_recordCount;
	Uint32	m_fieldCount;
	Uint32	m_stringSize;
	Uint32*	m_fieldsOffset;
	Uint8*	m_szFieldType;	

	Uint8*	m_data;
	Uint8*	m_stringTable;
	
	std::string	m_strFileName;
	std::vector<std::string>	m_fieldNameList;			// ¸÷ÁÐÃû³Æ
};

#ifdef WIN32
#pragma warning(pop)
#endif

END_MINIAPR_NAMESPACE

#endif
