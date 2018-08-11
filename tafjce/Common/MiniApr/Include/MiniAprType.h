/*********************************************************
*
*	名称: MiniArpType.h
*	作者: wuxf
*	时间: 2011-04-17
*	描述: MiniApr类型定义
*********************************************************/

#ifndef __MINI_APR_TYPE_H__
#define __MINI_APR_TYPE_H__

#include "servant/Application.h"

#define BEGIN_MINIAPR_NAMESPACE		namespace MINIAPR\
{


#define END_MINIAPR_NAMESPACE		}


BEGIN_MINIAPR_NAMESPACE
typedef		char				Int8;
typedef		unsigned char		Uint8;
typedef		short				Int16;
typedef		unsigned short		Uint16;
//typedef 	int					Int32;			
typedef		unsigned int		Uint32;
//typedef 	long				Int64;
typedef		unsigned long		Uint64;



template<class T1, class T2>
class Pair
{
	public:
		T1	val1;
		T2	val2;

	Pair(){};

	Pair(const Pair& _p)
		{
			this->val1 = _p.val1;
			this->val2 = _p.val2;
		};

	Pair(const T1& _val1, const T2& _val2)
		{
			this->val1 = _val1;
			this->val2 = _val2;
		};

	virtual ~Pair(){};

	virtual void operator = (const Pair& _p)
		{
			this->val1 = _p.val1;
			this->val2 = _p.val2;
		};
};


END_MINIAPR_NAMESPACE

#endif


