#ifndef _SINGLETON_20100930_H_
#define _SINGLETON_20100930_H_

// 单体模式模板

template <typename Type>

class CSingleton{
	public:
		static Type& Instance() { return m_oObject; }
	private:
		static Type 	m_oObject;
};

template <typename Type>
Type CSingleton<Type>::m_oObject;

#endif

