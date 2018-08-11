/*********************************************************
*
*	名称: Delegate.h
*	作者: wuxf
*	时间: 2009-03-03
*	描述: 代理类
*********************************************************/

#ifndef __APR_DELEGATE_H__
#define __APR_DELEGATE_H__

#include "TypeList/TypeList.h"
#include "RefCountAutoPtr/RefCountAutoPtr.h"


BEGIN_MINIAPR_NAMESPACE

namespace Detail
{
	
	template<class R, class TList>
	class FunctorBase
	{
	public:
		typedef R			ResultType;
		typedef MiniAprNullType Param1;
		typedef MiniAprNullType	 Param2;
		typedef MiniAprNullType Param3;
		typedef MiniAprNullType Param4;
		
		/*
		virtual ResultType operator()(Param1&) = 0;
		virtual ResultType operator()(Param1&, Param2&) = 0;
		virtual ResultType operator()(Param1&, Param2&, Param3&) = 0;
		virtual ResultType operator()(Param1&, Param2&, Param3&, Param4&) = 0;
		*/
	};

	template <unsigned int TListLength>struct FunctorHelper;

	template<>
	struct FunctorHelper<0>
	{
	public:
		
		template<class R, class TList>
		class In:public FunctorBase<R, TList>
		{
		public:
			 typedef R	ResultType;
			 
			 virtual ~In(){}
			 virtual ResultType operator()() = 0;
		};
	};

	template<>
	struct FunctorHelper<1>
	{
	public:

		template<class R, class TList>
		class In:public FunctorBase<R, TList>
		{
		public:
			typedef R	ResultType;
			typedef typename TypeAt<TList, 0>::Result	Param1;
			
			virtual ~In(){}
			virtual ResultType operator()(Param1& args) = 0;
		};
	};

	template<>
	struct FunctorHelper<2>
	{
	public:

		template<class R, class TList>
		class In:public FunctorBase<R, TList>
		{
		public:
			typedef R	ResultType;
			typedef typename TypeAt<TList, 0>::Result	Param1;
			typedef typename TypeAt<TList, 1>::Result	Param2;
			
			virtual ~In(){}
			virtual ResultType operator()(Param1& args1, Param2& args2) = 0;
		};
	};
	
	template<>
	struct FunctorHelper<3>
	{
	public:

		template<class R, class TList>
		class In:public FunctorBase<R, TList>
		{
		public:
			typedef R	ResultType;
			typedef typename TypeAt<TList, 0>::Result	Param1;
			typedef typename TypeAt<TList, 1>::Result	Param2;
			typedef typename TypeAt<TList, 2>::Result	Param3;
			
			virtual ~In(){}
			virtual ResultType operator()(Param1& args1, Param2& args2, Param3& args3) = 0;
		};
	};


	template<>
	struct FunctorHelper<4>
	{
	public:

		template<class R, class TList>
		class In:public FunctorBase<R, TList>
		{
		public:
			typedef R	ResultType;
			typedef typename TypeAt<TList, 0>::Result	Param1;
			typedef typename TypeAt<TList, 1>::Result	Param2;
			typedef typename TypeAt<TList, 2>::Result	Param3;
			typedef typename TypeAt<TList, 3>::Result	Param4;
			
			
			virtual ~In(){}
			virtual ResultType operator()(Param1& args1, Param2& args2, Param3& args3, Param4& args4) = 0;
		};
	};

	template<class R, class TList>
	class FunctorImp:public FunctorHelper<Length<TList>::value >::template In<R, TList>	
	{
	public:
		
		virtual FunctorImp* clone() = 0;
		virtual void* getThis() = 0;
		virtual string getMemFun() = 0;

		virtual ~FunctorImp(){}
	};
	
	class EventHandle
	{
	public:
		typedef RefcountAutoPtr<EventHandle> Proxy;
		EventHandle(){m_proxy.bind(this);}
		~EventHandle(){m_proxy.reset();}
		Proxy getEventHandle(){return m_proxy;}
	private:
		Proxy m_proxy;
	};
	
	// 类型是否可继承的检测，牛~~
	template<class T, class U>
	class ConvateType
	{
		typedef char Small;
		class Big{char szArray[2];};
		static Small ConvateFun(T);
		static Big ConvateFun(...);
		static U MakeU();
	public:
		enum
		{
			CHECK_RESULT = (sizeof(ConvateFun(MakeU() )) == sizeof(Small)),
		};
	};
	
	// 增加一个Bool参数，如果是从EventHandle继承的，就采用托管指针方式，否则，原生指针方式
	template <class U, class ResultType, class ArgsType, class MemFunType, bool bResult=false>
	class   MemFunHandle:public FunctorImp<ResultType, ArgsType>
	{
	public:
		
		typedef U CLASSTYPE; 

		typedef typename FunctorImp<ResultType, ArgsType>::Param1 Param1;
		typedef typename FunctorImp<ResultType, ArgsType>::Param2 Param2;
		typedef typename FunctorImp<ResultType, ArgsType>::Param3 Param3;
		typedef typename FunctorImp<ResultType, ArgsType>::Param4 Param4;
		
		/*MemFunHandle(U* pObj, MemFunType pFun):m_pObj(pObj),m_memFun(pFun)
		{
		}

		~MemFunHandle(){}

		ResultType operator()()
		{
			return (m_pObj->*m_memFun)();
		}

		ResultType operator()(Param1& args)
		{
			return (m_pObj->*m_memFun)(args);
		}

		ResultType operator()(Param1& args1, Param2& args2)
		{
			return (m_pObj->*m_memFun)(args1, args2);
		}

		ResultType operator()(Param1& args1, Param2& args2, Param3& args3)
		{
			return (m_pObj->*m_memFun)(args1, args2, args3);
		}

		ResultType operator()(Param1& args1, Param2& args2, Param3& args3, Param4& args4)
		{
			return (m_pObj->*m_memFun)(args1, args2, args3, args4);
		}

		MemFunHandle* clone() 
		{
			return new MemFunHandle(m_pObj, m_memFun);
		}

		virtual void* getThis()
		{
			return m_pObj;
		}

		virtual string getMemFun()
		{
			return string((char*)&m_memFun, sizeof(m_memFun) );
		}

	private:
		U* m_pObj;
		MemFunType m_memFun;*/
	};

	template <class U, class ResultType, class ArgsType,  class MemFunType>
	class   MemFunHandle<U, ResultType, ArgsType, MemFunType, true>:public FunctorImp<ResultType, ArgsType>
	{
	public:
		
		typedef U CLASSTYPE; 

		typedef typename FunctorImp<ResultType, ArgsType>::Param1 Param1;
		typedef typename FunctorImp<ResultType, ArgsType>::Param2 Param2;
		typedef typename FunctorImp<ResultType, ArgsType>::Param3 Param3;
		typedef typename FunctorImp<ResultType, ArgsType>::Param4 Param4;

		MemFunHandle(U* pObj, MemFunType pFun):m_handle( ( (EventHandle*)pObj)->getEventHandle() ), m_memFun(pFun)
		{	
		}

		MemFunHandle(EventHandle::Proxy handle, MemFunType pFun):m_handle(handle), m_memFun(pFun)
		{	
		}

		~MemFunHandle(){}

		ResultType operator()()
		{
			if(m_handle.get() )
			{
				return (m_handle.get()->*m_memFun)();
			}

			return ResultType();
		}

		ResultType operator()(Param1& args)
		{
			if(m_handle.get() )
			{
				return ( (U*)(m_handle.get())->*m_memFun)(args);
			}
			return ResultType();
		}

		ResultType operator()(Param1& args1, Param2& args2)
		{
			if(m_handle.get() )
			{
				return ( (U*)(m_handle.get())->*m_memFun)(args1, args2);
			}

			return ResultType();
		}

		ResultType operator()(Param1& args1, Param2& args2, Param3& args3)
		{
			if(m_handle.get() )
			{
				return ( (U*)(m_handle.get())->*m_memFun)(args1, args2, args3);
			}

			return ResultType();
		}

		ResultType operator()(Param1& args1, Param2& args2, Param3& args3, Param4& args4)
		{
			if(m_handle.get() )
			{
				return ( (U*)(m_handle.get())->*m_memFun)(args1, args2, args3, args4);
			}

			return ResultType();
		}

		MemFunHandle* clone() 
		{
			if(m_handle.get() == NULL)
			{
				//APR_ERROR("MemFunHandle clone Fail 对象不存在了");
			}
			return new MemFunHandle(m_handle, m_memFun);
		}

		virtual void* getThis()
		{
			return m_handle.get();
		}

		virtual string getMemFun()
		{
			return string((char*)&m_memFun, sizeof(m_memFun));
		}


	private:
		EventHandle::Proxy	m_handle;
		MemFunType m_memFun;
	};

	

	template <class ResultType, class ArgsType>
	class  Delegate 
	{
	public:
		typedef typename FunctorImp<ResultType, ArgsType>::Param1 Param1;
		typedef typename FunctorImp<ResultType, ArgsType>::Param2 Param2;
		typedef typename FunctorImp<ResultType, ArgsType>::Param3 Param3;
		typedef typename FunctorImp<ResultType, ArgsType>::Param4 Param4;

		Delegate(const Delegate& rhs)
		{
			m_functorHandle = rhs.m_functorHandle;
		}

		Delegate& operator = (const Delegate& rhs)
		{
			m_functorHandle = rhs.m_functorHandle;
			return *this;
		}

		
		~Delegate()
		{
		}
		
		ResultType operator()(Param1& args)const
		{
			return (*m_functorHandle.get() )(args);
		}		
		
		
		ResultType operator()(Param1& args1, Param2& args2) const
		{
			return (*m_functorHandle.get() )(args1, args2);
		}

		ResultType operator()(Param1& args1, Param2& args2, Param3& args3) const
		{
			return (*m_functorHandle.get() )(args1, args2, args3);
		}

		ResultType operator()(Param1& args1, Param2& args2, Param3& args3, Param4& args4) const
		{
			return (*m_functorHandle.get() )(args1, args2, args3, args4);
		}
		
		template<class U, class MemFunType>
		Delegate(U* pObj,MemFunType pFun)
		{
			FunctorImp<ResultType, ArgsType>* pTmpFunc = new MemFunHandle<U, ResultType, ArgsType, MemFunType, ConvateType<EventHandle, U>::CHECK_RESULT>(pObj, pFun); 
			m_functorHandle.bind(pTmpFunc);
		}

		bool operator == (const Delegate& rhs)const
		{
			return (!(*this < rhs) ) && (!(rhs < *this) ); 
		}

		bool operator < (const Delegate& rhs) const
		{
			assert(m_functorHandle.get() && rhs.m_functorHandle.get() );
			
			if(m_functorHandle.get()->getThis() != rhs.m_functorHandle.get()->getThis() )
			{
				return m_functorHandle.get()->getThis() < rhs.m_functorHandle.get()->getThis();
			}
			
			string selfFunPoint = m_functorHandle.get()->getMemFun();
			string rhsFunPoint = rhs.m_functorHandle.get()->getMemFun();
			
			return selfFunPoint < rhsFunPoint;
		}

	private:
		
		RefcountAutoPtr<FunctorImp<ResultType, ArgsType>, true> m_functorHandle;
	};
};

END_MINIAPR_NAMESPACE


#endif

