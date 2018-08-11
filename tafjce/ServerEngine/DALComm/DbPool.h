//////////////////////////////////////////////////////////////////////////
//    DB连接对象池
//    author: forrestliu@tencent.com 
//////////////////////////////////////////////////////////////////////////

#ifndef __DB_POOL_H__
#define __DB_POOL_H__

#include <iosfwd>
#include "util/tc_config.h"
#include "util/tc_singleton.h"
#include "util/tc_thread_queue.h"
#include "util/tc_common.h"
#include "util/tc_functor.h"
#include "util/tc_thread_pool.h"

using namespace taf;

namespace ServerEngine
{
/** default pool factory
 */
template < typename T >
struct default_pool_factory
{
    #define MIN_RENEW_SPAN 3
    void init(const map<string,string>& conf)
    {
        _conf = conf;
        _timeStamp = time(NULL);
        _conf["time-stamp"] = TC_Common::tostr(_timeStamp);
    }
    
	bool is_valid(T * e)	
    { 
        return e->isValid(_timeStamp); 
    }
    
	T * create()	
    { 
        return new T(_conf); 
    }
    
	void destory(T * e)		
    { 
        delete e; 
    }

    void renew()
    {
        if(time(NULL) - _timeStamp > MIN_RENEW_SPAN)
        {
            _timeStamp = time(NULL);
        }
    }
private:
    map<string,string> _conf;
    int                _timeStamp;
};

/** default pool resize policy
 */
struct default_pool_policy
{
	typedef size_t	size_type;

	/** get grow count
	 *  \param pool_size	pool size
	 *  \param min_size		min object size
	 *  \param max_size		max object size
	 *  \param free_count	free object count
	 *  \return				if pool need grow, return a positive number;
	 *  					if need to reduce, return a negative number;
	 *  					otherwise, return zero;
	 */
	int get_grow_count(size_type pool_size, size_type min_size, size_type max_size, size_type free_count);
};

/** db pool
 */
template < typename T, 
		 typename Factory = default_pool_factory<T>, 
		 typename BlockingQueue = TC_ThreadQueue<T*>,
		 typename Policy = default_pool_policy >
class DbPool : public TC_Singleton<DbPool<T, Factory, BlockingQueue, Policy> >
{
public:
	typedef T									            value_type;
	typedef Factory								            factory_type;
	typedef BlockingQueue						            queue_type;
	typedef Policy								            policy_type;
	typedef typename BlockingQueue::queue_type::size_type	size_type;

    typedef void (DbPool::*CreatePtr)();
    typedef void (DbPool::*DestoryPtr)(void* e);
    
protected:
	BlockingQueue		    _queue;			///< object queue
	Factory				    _factory;		///< object factory
	Policy				    _policy;		///< resize policy
	TC_ThreadPool		    _tp;			///< thread pool to create/destory object
	volatile size_type	    _pool_size;		///< pool's object count
	TC_ThreadMutex          _mutex_ps;		///< locker for _pool_size
	size_type			    _min_size;		///< min count of object
	size_type			    _max_size;		///< max count of object

    bool                    _is_inited;            ///<是否已经初始化
	
public:
    /** 
     * constructor
	 */
	DbPool():_pool_size(0),_min_size(0),_max_size(0),_is_inited(false){};


	/** initialize
	 *  \param min_size			max count of object
	 *  \param max_size			min count of object
	 *  \param thread_count		thread count of create/destory thread pool
	 */
	void init(const map<string,string> conf);

    void init(const TC_Config& conf);

	/// create an object and push it to pool
	void create_one();

    /// destory an object from the pool
    void destory_one(void* e);

	/// get pool's object count
	size_type pool_size() const		{ return _pool_size; }

	/// get min object count
	size_type min_size() const		{ return _min_size; }

	/// get max object count
	size_type max_size() const		{ return _max_size; }

	/// get free object count
	size_type free_count() const	{ return _queue.size(); }

	/// have some object free?
	bool empty() const				{ return _queue.empty(); }

	/// pop an object
	T * pop();

	/// push an object
	void push(T * e);

    void renew() {_factory.renew();}
private:
	int clean_up();
};

template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy,
		 typename charT, 
		 typename traits>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& ost, const DbPool<T, Factory, BlockingQueue, Policy>& pool)
{
	ost << "{ pool_size: " << pool.pool_size()
		<< ", min_size: " << pool.min_size()
		<< ", max_size: " << pool.max_size()
		<< ", free_count: " << pool.free_count()
		<< "}";
	return ost;
}

//////////////////////////////////////////////////////////////////////////
// implementations

inline int default_pool_policy::get_grow_count(size_type pool_size, size_type min_size, size_type max_size, size_type free_count)
{
	if(pool_size < max_size && free_count <= 1)
		return 1;
	if(pool_size > min_size && pool_size - free_count < min_size / 2)
		return - static_cast<int>(pool_size - min_size) / 2;
	return 0;
}
/////////////////////////////////////////////////////////////////////////
// DbPool
template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy >
inline void DbPool<T, Factory, BlockingQueue, Policy>::init(const map<string,string> conf)
{
    if(!_is_inited)
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex_ps);
        if(!_is_inited)
        {
            map<string,string>::const_iterator it = conf.begin();
            it = conf.find("MinSize");
            if(it != conf.end())
            {
                _pool_size = _min_size = TC_Common::strto<size_t>(it->second);
            }
            else
            {
                _pool_size = _min_size = 1;
            }

            it = conf.find("MaxSize");
            if(it != conf.end())
            {
                _max_size = TC_Common::strto<size_t>(it->second);
            }
            else
            {
                _max_size = _min_size;
            }

            _factory.init(conf);

            _tp.init(2);
            _tp.start();

            TC_Functor<void> create_one(this,static_cast<CreatePtr>(&DbPool::create_one));
            TC_Functor<void>::wrapper_type fw(create_one);
	        for(size_type i = 0; i < _pool_size; ++i)
	        {
		        _tp.exec(fw);
	        }

            _is_inited = true;
        }   
    }
    
}

template < typename T, 
           typename Factory, 
           typename BlockingQueue,
           typename Policy >
inline void DbPool<T, Factory, BlockingQueue, Policy>::init(const TC_Config& conf)
{
   const map<string,string> mapConf = conf.getDomainMap("/DbPool");
   init(mapConf);    
}


template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy >
inline void DbPool<T, Factory, BlockingQueue, Policy>::create_one()
{
	T * e = _factory.create();
	_queue.push_back(e);
}

template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy >
inline void DbPool<T, Factory, BlockingQueue, Policy>::destory_one(void* e)
{
	_factory.destory((T*)e);
}

template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy >
inline T * DbPool<T, Factory, BlockingQueue, Policy>::pop()
{
	clean_up();
	while(_is_inited) {
		T * e = NULL;
        bool bRet = _queue.pop_front(e,-1);
		if(bRet && _factory.is_valid(e))
		{
			return e;
		}

        TC_Functor<void> create_one(this,static_cast<CreatePtr>(&DbPool::create_one));
        TC_Functor<void>::wrapper_type cfw(create_one);
		_tp.exec(cfw);

        TC_Functor<void,TL::TLMaker<void*>::Result> destory_one(this,static_cast<DestoryPtr>(&DbPool::destory_one));
        TC_Functor<void,TL::TLMaker<void*>::Result>::wrapper_type dfw(destory_one,e);
		_tp.exec(dfw);
	}

    return NULL;
}

template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy >
inline void DbPool<T, Factory, BlockingQueue, Policy>::push(T * e)
{
	if(_is_inited && _factory.is_valid(e))
    {
		_queue.push_back(e);
	}
    else
    {
        TC_Functor<void> create_one(this,static_cast<CreatePtr>(&DbPool::create_one));
        TC_Functor<void>::wrapper_type cfw(create_one);
		_tp.exec(cfw);

        TC_Functor<void,TL::TLMaker<void*>::Result> destory_one(this,static_cast<DestoryPtr>(&DbPool::destory_one));
        TC_Functor<void,TL::TLMaker<void*>::Result>::wrapper_type dfw(destory_one,e);
		_tp.exec(dfw);
	}
}

template < typename T, 
		 typename Factory, 
		 typename BlockingQueue,
		 typename Policy >
inline int DbPool<T, Factory, BlockingQueue, Policy>::clean_up()
{
    int gn = 0;
    //cout<<"before clean_up:"<<gn<<"|"<<pool_size()<<"|"<<min_size()<<"|"<<max_size()<<"|"<<_queue.size()<<endl;
	
    {
        TC_LockT<TC_ThreadMutex> lock(_mutex_ps);
	    gn = _policy.get_grow_count(pool_size(), min_size(), max_size(), _queue.size());
	    _pool_size += gn;
    }
	if(gn == 0)
	{
		return 0;
	}
    
	if(gn > 0)
    {
		for(int i = 0; i < gn; ++i)
		{
			TC_Functor<void> create_one(this,static_cast<CreatePtr>(&DbPool::create_one));
            TC_Functor<void>::wrapper_type cfw(create_one);
		    _tp.exec(cfw);
		}
	}
    else
    {
		for(int i = 0; i > gn; --i)
        {
			T * e = NULL;
            _queue.pop_front(e,-1);

			TC_Functor<void,TL::TLMaker<void*>::Result> destory_one(this,static_cast<DestoryPtr>(&DbPool::destory_one));
            TC_Functor<void,TL::TLMaker<void*>::Result>::wrapper_type dfw(destory_one,e);
		    _tp.exec(dfw);
		}
	}
    
    //cout<<"after clean_up:"<<gn<<"|"<<pool_size()<<"|"<<min_size()<<"|"<<max_size()<<"|"<<_queue.size()<<endl;

	return gn;
}

/** class to helper pop/push an object from a pool.
 */
template < typename T, 
		 typename Factory = default_pool_factory<T>,
		 typename BlockingQueue = TC_ThreadQueue<T*>,
		 typename Policy = default_pool_policy >
class pool_object
{
	DbPool<T, Factory, BlockingQueue>& _pl;	///< pool
	T * _e;									///< object pop from pool

	pool_object(const pool_object&);
	pool_object& operator=(const pool_object&);
public:
	pool_object(DbPool<T, Factory, BlockingQueue>& pl)
		: _pl(pl)
		, _e(_pl.pop())
	{}

	pool_object(DbPool<T, Factory, BlockingQueue>* pl)
		: _pl(*pl)
		, _e(_pl.pop())
	{}

	~pool_object() { _pl.push(_e); }

	operator T*()	{ return _e; }
	T& operator*()	{ return *_e; }
	T* operator->()	{ return _e; }
	T* get()		{ return _e; }
};

}

#endif
