#ifndef _HASH_DFT_FUNCTION_20100919_H
#define _HASH_DFT_FUNCTION_20100919_H

template<typename TKEY>
struct  STDDefaultKey{
   
    int operator()(const TKEY &key) const { return  key;}
};


template<typename TKEY>
struct STDDefaultCompare{
	bool operator()(const TKEY &key1, const TKEY &key2) const{return key1 == key2;}
};

#endif

