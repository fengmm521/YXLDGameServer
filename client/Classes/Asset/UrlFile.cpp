
#include "UrlFile.h"

NS_FM_BEGIN

UrlFile::UrlFile( const char* tag )
{
	m_TagName = tag;
	m_NeedRefresh = false;
	m_NeedDel = false;
	m_isPcakRes = false;
}

UrlFile::UrlFile()
{
	m_NeedRefresh = false;
	m_NeedDel = false;
	m_isPcakRes = false;
}

UrlFile::~UrlFile()
{

}



NS_FM_END