#ifndef _OUTER_PROXY_FACTORY_H_
#define _OUTER_PROXY_FACTORY_H_

#include <map>
#include <stdexcept>
#include "util/tc_autoptr.h"
#include "util/tc_thread_mutex.h"
#include "servant/Global.h"
#include "servant/Application.h"

using namespace std;
using namespace taf;
/**
 * 代理对象工厂
 */
class OuterProxyFactory : public TC_HandleBase
                        , public TC_ThreadMutex
{
public:
	template<class T> T& getChecked(const string& name, T& t)
	{
		if (_proxy.find(name) == _proxy.end())
		{
			t = Application::getCommunicator()->stringToProxy<T>(name);
            //在TAF下这个判断不会进入，TAF获取对象代理不会执行远端检测
			if (!t)
			{
                ostringstream os; 
                os << "invalid outer proxy:" << name;

				throw TC_Exception(os.str().c_str(), __LINE__);
			}
            else
            {
                TC_LockT<TC_ThreadMutex> lock(*this);
			    _proxy[name] = t;
            }
            
			return t;
		}
        
        t = (typename T::element_type*)((_proxy[name]).get());
        
        return t;
	}

private:
	OuterProxyFactory(){}

	friend class OuterFactoryImp;

private:
	map<string, taf::ServantPrx> _proxy;
};

typedef TC_AutoPtr<OuterProxyFactory> OuterProxyFactoryPtr;
/////////////////////////////////////////////////////////////////
#endif
