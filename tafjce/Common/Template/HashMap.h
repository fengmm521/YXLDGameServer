#ifndef _HASH_MAP_20100916_H_
#define _HASH_MAP_20100916_H_

// 对suse 、slackware linux 不同版本linux的HashMap的支持

#if __GNUC__ > 2
#include        <ext/hash_map>
#define         HashMap     __gnu_cxx::hash_map
#else
#include        <hash_map>
#define         HashMap     std::hash_map
#endif

#endif

