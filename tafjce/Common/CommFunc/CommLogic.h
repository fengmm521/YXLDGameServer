#ifndef _COMM_LOGIC_H_
#define _COMM_LOGIC_H_
/////////////////////////////////////////////////////////////////////
#include <math.h>
#include <set>
#include "servant/Application.h"
#include "servant/Communicator.h"
#include "count_time.hpp"
#include "ErrorHandle.h"

/////////////////////////////////////////////////////////////////////
using namespace std;
using namespace taf;

/////////////////////////////////////////////////////////////////////

namespace ServerEngine
{
/////////////////////////////////////////////////////////////////////

/**
 * 通用模板方法: JCE对象T序列化
 * @param t 类型T的对象
 * @return 字符串
 */
template<typename T> std::string JceToStr(const T& t)
{
	string s;
	try
	{
	    taf::JceOutputStream<BufferWriter> osk;
	    t.writeTo(osk);
	    s.assign(osk.getBuffer(),osk.getLength());
	}
	catch(std::exception& e)
	{
		string strReason = e.what();
		assert(false);
	}

    return s;
}

/**
 * 通用模板方法: JCE对象T结构化
 * @param s 字符串
 * @return 类型T的对象
 */
template<typename T> T& JceToObj(const std::string& s,T& t)
{
	try
	{
    	taf::JceInputStream<BufferReader> isk;
		isk.setBuffer(s.c_str(),s.length());
		t.readFrom(isk);
	}
	catch(std::exception& e)
	{
		string strReason = e.what();
		assert(false);
	}

    return t;
}

/**
 * 通用模板方法: JCE对象T序列化
 * @param t 类型T的对象
 * @param buffer 序列化后的字符数组
 */
template<typename T> void JceToBuffer(const T& t, vector<taf::Char>& buffer)
{
    taf::JceOutputStream<BufferWriter> osk;
    t.writeTo(osk);
    buffer = osk.getByteBuffer();

    return;
}

/**
 * 通用模板方法: JCE对象T结构化
 * @param vc 字符数组
 * @param t 类型T的对象
 * @return 类型T的对象
 */
template<typename T, typename B> T& JceToObj(const B& vc,T& t)
{
    taf::JceInputStream<BufferReader> isk;
    isk.setBuffer((const taf::Char*)(&vc[0]), vc.size());
    t.readFrom(isk);

    return t;
}
/**
 * 通用模板方法: JCE对象T结构化打印
 * @param t 类型T的对象
 * @return 字符串
 */
template<typename T> std::string PrintJce(const T& t)
{
    ostringstream os;
    t.display(os);

    return os.str();
}

/**
 * 通用模板方法: Jce对象容器结构化打印
 * @param itbegin  容器的iterator begin
 * @param itend  容器的iterator end
 * @return 字符串
 */
template <typename I> std::string PrintJce(I itbegin,I itend)
{
    std::ostringstream s;
    I it = itbegin;
	while(it!=itend) 
    {
		it->display(s);
        it++;
	}
	return s.str();
}

/**
 * 通用模板方法: JCE对象T单行日志化打印
 * @param t 类型T的对象
 * @return 字符串
 */
template<typename T> std::string LogJce(const T& t)
{
    ostringstream os;
    t.displaySimple(os);

    return os.str();
}

/**
 * 通用模板方法: Jce对象容器日志化打印
 * @param itbegin  容器的iterator begin
 * @param itend  容器的iterator end
 * @return 字符串
 */
template <typename I> std::string LogJce(I itbegin,I itend)
{
    std::ostringstream s;
    I it = itbegin;
	while(it!=itend) 
    {
		it->displaySimple(s);
        s<<";";
        it++;
	}
	return s.str();
}


//类型转换
#define TC_I2S(I)       TC_Common::tostr<int>(I)
#define TC_S2I(S)       TC_Common::strto<int>(S)
#define TC_S2SIZET(S)   TC_Common::strto<size_t>(S)
#define TC_S2KEYT(S)    TC_Common::strto<key_t>(S)
#define TC_TIMET2S(I)   TC_Common::tostr<time_t>(I)
#define TC_S2TIMET(I)   TC_Common::strto<time_t>(I)
#define TC_S2F(S)       TC_Common::strto<float>(S)
#define TC_U2S(N)       TC_Common::tostr<unsigned>(N)
#define TC_S2U(S)       TC_Common::strto<unsigned>(S)
#define TC_TRIM(S)      TC_Common::trim(S)
#define TC_BIN2S(S)     TC_Common::bin2str(S)
#define TC_G2U8(S)      TC_Encoder::gbk2utf8(S)
#define TC_U82G(S)      TC_Encoder::utf82gbk(S)

//过程控制
#define PROC_BEGIN do{
#define PROC_END   }while(0);

#define PROC_TRY_BEGIN do{try{
#define PROC_TRY_END   }__COMMON_EXCEPTION_CATCH__}while(0);

inline unsigned int StrIp2Int(const string& sIp)
{
	struct in_addr inaddr;
	inet_aton(sIp.c_str(), &inaddr);
	return inaddr.s_addr;
}
inline bool IsQQ(unsigned i)
{
    return (i > 10000) && (i < 0x80000000);
}

inline bool IsQQ(const string& qq)
{
	string sQQ = TC_TRIM(qq);
	if (TC_Common::isdigit(sQQ))
	{
		unsigned i = TC_S2U(sQQ);
    	return IsQQ(i);
	}

	return false;
}
inline void StringToLower(const string& sOrg, string& sDest)
{
    sDest = sOrg;
	transform(sDest.begin(),sDest.end(),sDest.begin(), ::tolower);
}

/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
#endif //
/////////////////////////////////////////////////////////////////////




