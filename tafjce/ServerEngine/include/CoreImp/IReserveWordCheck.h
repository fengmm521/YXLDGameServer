#ifndef __IRESERVE_WORD_CHECK_H__
#define __IRESERVE_WORD_CHECK_H__


class IReserveWordCheck:public IComponent
{
public:

	virtual int getWordLength(const string& strMsg) = 0;

	virtual bool hasReserveWord(const string& strMsg) = 0;

	virtual size_t calLength(const char* src) const = 0;
};




#endif
