#ifndef __RESERVE_WORDCHECK_H__
#define __RESERVE_WORDCHECK_H__

class ReserveWordCheck:public ComponentBase<IReserveWordCheck, IID_IReserveWordCheck>
{
public:

	ReserveWordCheck();
	~ReserveWordCheck();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);
	
	// IReserveWordCheck interface
	virtual int getWordLength(const string& strMsg);
	virtual bool hasReserveWord(const string& strMsg);

	void loadExtraReserverName(const string& strTableName, const string& strFiledName);
	size_t calLength(const char* src) const;

private:

	set<string> m_reserveWordList;
};



#endif
