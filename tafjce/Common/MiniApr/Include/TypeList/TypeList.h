#ifndef __TYPE_LIST_H__
#define __TYPE_LIST_H__

BEGIN_MINIAPR_NAMESPACE

template<class U, class T>
struct TypeList
{
	typedef U Head;
	typedef T Tail;
};

struct MiniAprNullType
{
};


#define APR_TYPELIST_1(T1)	TypeList<T1, MINIAPR::MiniAprNullType>

#define APR_TYPELIST_2(T1, T2)	TypeList<T1, APR_TYPELIST_1(T2)>

#define APR_TYPELIST_3(T1, T2, T3)	TypeList<T1, APR_TYPELIST_2(T2, T3)>

#define APR_TYPELIST_4(T1, T2, T3, T4)	TypeList<T1, APR_TYPELIST_3(T2, T3, T4)>

template<class TList, unsigned int index> 
struct TypeAt
{
	typedef MiniAprNullType Result;
};

template<class T>
struct TypeAt<T, 0>
{
	typedef T Result;
};

template<class Head, class Tail>
struct TypeAt<TypeList<Head, Tail>, 0>
{
	typedef Head Result;
};


template<class Head, class Tail, unsigned int i>
struct TypeAt<TypeList<Head, Tail>, i>
{
	typedef typename TypeAt<Tail, i - 1>::Result Result;
};

template<class TList> struct Length;

template<>struct Length<MiniAprNullType>
{
	enum{value = 0};;
};

template<class T, class U>
struct Length<TypeList<T, U> >
{
	enum{value  = 1 + Length<U>::value};
};

END_MINIAPR_NAMESPACE


#endif

