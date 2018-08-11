#ifndef _HASH_MAP_FIXED_20100919_H_
#define _HASH_MAP_FIXED_20100919_H_

#include    <stdlib.h>
#include    "objectpool.h"
#include    "hash_dft_func.h"

/*
    :使用堆进行限量分配的hashmap,init后无动态内存的再分配

    使用说明:
    1.以int,short,char,long (singed or unsigned)作为Key,可自动使用默认的hash计算函数
    2.对自定义struct作为Key,必须提供hash计算函数，方法与std::hash_map相同
    3.对自定义struct作为Key,必须定义struct的operator==()判断操作
    4.对自定义struct作为Key、Val,struct可以定义operator=()赋值操作.
    5.对自定义struct作为Key、Val,Key和Val需要支持默认的无参数类型的构造函数
*/


//template<typename TKEY,typename TVAL,typename TKEY2HASHVAL = STDDefaultKey<TKEY>,typename TKEYCOMPARE = STDDefaultCompare<TKEY> >
template<typename TKEY,typename TVAL,typename TKEY2HASHVAL = STDDefaultKey<TKEY>,typename TKEYCOMPARE = STDDefaultCompare<TKEY> >
class   CHashMapFixed
{

	public:
	    CHashMapFixed():m_iIndexNum(0),m_pstMapDataNodeListHeadPtrs(NULL),
	                    m_iMaxDataNum(0),m_pstMapDataNodeRoot(NULL),
	                    m_pstMapDataNodeTimeListHead(NULL),m_pstMapDataNodeTimeListIter(NULL)
	    {
	        m_sErrMsg[255] = 0;
	    }

	    ~CHashMapFixed()
	    {
	        if (m_pstMapDataNodeListHeadPtrs)
	            delete  []m_pstMapDataNodeListHeadPtrs;

	        if (m_pstMapDataNodeRoot)
	            delete  []m_pstMapDataNodeRoot;
	    }


	    /*
	        Init(iIndexNum,iMaxDataNum):初始化,决定hash_map_fixed占用的固定空间大小

	        iIndexNum : hash值对应的int数组大小
	        iMaxDataNum: 最大存储Key&Value对的个数

	    */
	    int Init(int iIndexNum,int iMaxDataNum)
	    {
	        if (m_iIndexNum != 0 || m_iMaxDataNum != 0)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::init fail:init already...");
	            return  -1;
	        }

	        if (iIndexNum <= 0)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::init fail:invalid index_num<%d>",iIndexNum);
	            return  -1;
	        }

	        if (iMaxDataNum <= 0)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::init fail:invalid max_data_num<%d>",iMaxDataNum);
	            return  -1;
	        }

	        m_iIndexNum = iIndexNum;
	        m_iMaxDataNum = iMaxDataNum;

	        m_pstMapDataNodeListHeadPtrs = new STMapDataNodePtr[m_iIndexNum];

	        // IndexArray
	        for (int i = 0;i < m_iIndexNum;i++)
	            m_pstMapDataNodeListHeadPtrs[i] = NULL;

	        m_oEntireMapDataNodePool.InitPool(m_iMaxDataNum);

	        m_pstMapDataNodeRoot = new STMapDataNode[m_iMaxDataNum];
	        for (int i = 0;i < m_iMaxDataNum;i++)
	            m_oEntireMapDataNodePool.ObjectBackPush(m_pstMapDataNodeRoot + i);

	        return  0;
	    }



	  
	    int Add(const TKEY &Key,const TVAL &Val)
	    {
	        if (!m_pstMapDataNodeListHeadPtrs)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::Add fail:not init yet...");
	            return  -1;
	        }

	        TKEY2HASHVAL  tKey2HashValFcn;
	        TKEYCOMPARE   tKeyCompFcn;

	        int iHashVal = abs(tKey2HashValFcn(Key)) % m_iIndexNum;
	        STMapDataNodePtr pstMapDataNode = m_pstMapDataNodeListHeadPtrs[iHashVal];

	        while (pstMapDataNode)
	        {
	            // Key相等?则做Update
	            if (tKeyCompFcn(pstMapDataNode->m_data.m_Key,Key))
	            {
	                pstMapDataNode->m_data.m_Val = Val;
	                return  0;
	            }
	            pstMapDataNode = pstMapDataNode->m_pNext;
	        }

	        STMapDataNodePtr pstFreeMapDataNode = m_oEntireMapDataNodePool.ObjectFrontPop();
	        if (!pstFreeMapDataNode)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::Add fail:no more space");
	            return  -1;
	        }

	        pstFreeMapDataNode->m_data.m_Key = Key;
	        pstFreeMapDataNode->m_data.m_Val = Val;
	        pstFreeMapDataNode->m_pNext = m_pstMapDataNodeListHeadPtrs[iHashVal];
	        pstFreeMapDataNode->m_pTimeNxt = m_pstMapDataNodeTimeListHead;
	        pstFreeMapDataNode->m_pTimePre = NULL;

	        if (pstFreeMapDataNode->m_pTimeNxt)
	            pstFreeMapDataNode->m_pTimeNxt->m_pTimePre = pstFreeMapDataNode;

	        m_pstMapDataNodeListHeadPtrs[iHashVal] = pstFreeMapDataNode;
	        m_pstMapDataNodeTimeListHead = pstFreeMapDataNode;

	        return  0;
	    }


	    int Search(const TKEY &tKey,TVAL &tVal)
	    {
	        TVAL    *ptVal;

	        if (Search(tKey,ptVal) < 0)
	            return  -1;

	        tVal = *ptVal;

	        return  0;
	    }


	    int Search(const TKEY &tKey,TVAL *&ptVal)
	    {
	        ptVal = NULL;

	        if (!m_pstMapDataNodeListHeadPtrs){
	            snprintf(m_sErrMsg,255,"CHashMapFixed::Search fail:not init yet...");
	            return  -1;
	        }

	        TKEY2HASHVAL  tKey2HashValFcn;
	        TKEYCOMPARE   tKeyCompFcn;

	        int iHashVal = abs(tKey2HashValFcn(tKey)) % m_iIndexNum;
	        STMapDataNodePtr pstMapDataNode = m_pstMapDataNodeListHeadPtrs[iHashVal];

	        while (pstMapDataNode) {
	            if (tKeyCompFcn(pstMapDataNode->m_data.m_Key,tKey)){
	                ptVal = &pstMapDataNode->m_data.m_Val;
	                return  0;
	            }
	            pstMapDataNode = pstMapDataNode->m_pNext;
	        }

	        snprintf(m_sErrMsg,255,"CHashMapFixed::Search fail:not found");
	        return  -1;
	    }



        //Erase(Key):删除Key对应的Key&Value对
	    int Erase(const TKEY &tKey)
	    {
	        if (!m_pstMapDataNodeListHeadPtrs) {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::Erase fail:not init yet...");
	            return  -1;
	        }

	        TKEY2HASHVAL  tKey2HashValFcn;
	        TKEYCOMPARE   tKeyCompFcn;

	        int iHashVal = abs(tKey2HashValFcn(tKey)) % m_iIndexNum;
	        // 对应得Index桶是空的
	        if (!m_pstMapDataNodeListHeadPtrs[iHashVal])
	            return  0;

	        STMapDataNodePtr pstMapDataNode = m_pstMapDataNodeListHeadPtrs[iHashVal];

	        if (tKeyCompFcn(pstMapDataNode->m_data.m_Key,tKey))
	        {
	            m_pstMapDataNodeListHeadPtrs[iHashVal] = pstMapDataNode->m_pNext;

	            pstMapDataNode->m_data.~STKeyValPair();

	            if (pstMapDataNode->m_pTimeNxt)
	                pstMapDataNode->m_pTimeNxt->m_pTimePre = pstMapDataNode->m_pTimePre;
	            if (pstMapDataNode->m_pTimePre)
	                pstMapDataNode->m_pTimePre->m_pTimeNxt = pstMapDataNode->m_pTimeNxt;
	            
	            if (m_pstMapDataNodeTimeListHead == pstMapDataNode)
	                m_pstMapDataNodeTimeListHead = pstMapDataNode->m_pTimeNxt;

	            if (m_pstMapDataNodeTimeListIter == pstMapDataNode)
	                m_pstMapDataNodeTimeListIter = pstMapDataNode->m_pTimeNxt;

	            m_oEntireMapDataNodePool.ObjectBackPush(pstMapDataNode);

	            return  0;
	        }

	        STMapDataNodePtr pstMapDataNodePre;
	        while (1)
	        {
	            pstMapDataNodePre = pstMapDataNode;
	            pstMapDataNode = pstMapDataNode->m_pNext;

	            if (!pstMapDataNode)
	                break;

	            if (tKeyCompFcn(pstMapDataNode->m_data.m_Key,tKey))
	            {
	                pstMapDataNodePre->m_pNext = pstMapDataNode->m_pNext;

	                pstMapDataNode->m_data.~STKeyValPair();

	                if (pstMapDataNode->m_pTimeNxt)
	                    pstMapDataNode->m_pTimeNxt->m_pTimePre = pstMapDataNode->m_pTimePre;
	                if (pstMapDataNode->m_pTimePre)
	                    pstMapDataNode->m_pTimePre->m_pTimeNxt = pstMapDataNode->m_pTimeNxt;
	                
	                if (m_pstMapDataNodeTimeListHead == pstMapDataNode)
	                    m_pstMapDataNodeTimeListHead = pstMapDataNode->m_pTimeNxt;

	                if (m_pstMapDataNodeTimeListIter == pstMapDataNode)
	                    m_pstMapDataNodeTimeListIter = pstMapDataNode->m_pTimeNxt;
	                
	                m_oEntireMapDataNodePool.ObjectBackPush(pstMapDataNode);
	                break;
	            }
	        }

        	return  0;
    	}



	   	// 清除hash_map对应的当前所有数据
	    void    Clear()
	    {
	        STMapDataNodePtr    pstMapDataNode;

	        if (!m_pstMapDataNodeListHeadPtrs)
	            return;

	        for (int i = 0;i < m_iIndexNum;i++)   {
	            pstMapDataNode = m_pstMapDataNodeListHeadPtrs[i];
	            while (pstMapDataNode)  {
	                pstMapDataNode->m_data.~STKeyValPair();

	                m_oEntireMapDataNodePool.ObjectBackPush(pstMapDataNode);
	                pstMapDataNode = pstMapDataNode->m_pNext;
	            }
	            m_pstMapDataNodeListHeadPtrs[i] = NULL;
	        }
	        m_pstMapDataNodeTimeListHead = NULL;
	        m_pstMapDataNodeTimeListIter = NULL;
	    }

        //LinearQueryBegin():线性查找开始,rewind查找指针到时间链表头
	    void  LinearQueryBegin(){
	        m_pstMapDataNodeTimeListIter = m_pstMapDataNodeTimeListHead;
	    }

	    //    GetNext():根据时间链表,获取当前数据,并自动位移指针到下一个位置返回Map中对应数据得拷贝
	    int   GetNext(TKEY &tKey,TVAL &tVal)
	    {
	        TKEY    *ptKey;
	        TVAL    *ptVal;

	        if (GetNext(ptKey,ptVal) < 0)
	            return  -1;

	        tKey = *ptKey;
	        tVal = *ptVal;

	        return  0;
	    }

	    //    GetNext():根据时间链表,获取当前数据,并自动位移指针到下一个位置 返回Map中对应数据得拷贝 若当前返回的是时间链表中第一个数据,则bIsFirst为true
	    int   GetNext(TKEY &tKey,TVAL &tVal,bool &bIsFirst)
	    {
	        TKEY    *ptKey;
	        TVAL    *ptVal;

	        if (GetNext(ptKey,ptVal,bIsFirst) < 0)
	            return  -1;

	        tKey = *ptKey;
	        tVal = *ptVal;

	        return  0;
	    }

	    /*
	        GetNext():根据时间链表,获取当前数据,并自动位移指针到下一个位置
	                  返回Map中对应数据得地址,从效率上来说,比上面GetNext()要高
	                  TKEY *& ,TVAL *& 入口参数指针的引用
	    */
	    int     GetNext(TKEY *&ptKey,TVAL *&ptVal)
	    {
	        ptKey = NULL;
	        ptVal = NULL;

	        if (!m_pstMapDataNodeTimeListIter)
	            return  -1;

	        ptKey = &m_pstMapDataNodeTimeListIter->m_data.m_Key;
	        ptVal = &m_pstMapDataNodeTimeListIter->m_data.m_Val;

	        m_pstMapDataNodeTimeListIter = m_pstMapDataNodeTimeListIter->m_pTimeNxt;
	        return  0;
	    }

	    /*
	        GetNext():根据时间链表,获取当前数据,并自动位移指针到下一个位置
	                  返回Map中对应数据得地址,从效率上来说,比上面GetNext()要高
	                  TKEY *& ,TVAL *& 入口参数指针的引用
	                  若当前返回的是时间链表中第一个数据,则bIsFirst为true
	    */
	    int     GetNext(TKEY *&ptKey,TVAL *&ptVal,bool &bIsFirst)
	    {
	        ptKey = NULL;
	        ptVal = NULL;

	        if (!m_pstMapDataNodeTimeListIter)
	            return  -1;

	        ptKey = &m_pstMapDataNodeTimeListIter->m_data.m_Key;
	        ptVal = &m_pstMapDataNodeTimeListIter->m_data.m_Val;

	        bIsFirst = false;

	        if (m_pstMapDataNodeTimeListIter == m_pstMapDataNodeTimeListHead)
	            bIsFirst = true;

	        m_pstMapDataNodeTimeListIter = m_pstMapDataNodeTimeListIter->m_pTimeNxt;
	        
	        return  0;
	    }


    	const char *GetErrMsg() const { return  m_sErrMsg; }


	    /*
	        GetOjbNum():当前已存储的数据个数
	    */
	    int GetObjectNum() const
	    {
	        return  m_oEntireMapDataNodePool.GetFreeCapacity();
	    }

	    int GetCapacity() const
	    {
	        return  m_iMaxDataNum;
	    }


 
	    int GetKey2ValPairByPos(int iPos,TKEY &tKey,TVAL &tVal)
	    {
	        TKEY    *ptKey;
	        TVAL    *ptVal;

	        if (GetKey2ValPairByPos(iPos,ptKey,ptVal) < 0)
	            return  -1;

	        tKey = *ptKey;
	        tVal = *ptVal;

	        return  0;
	    }

        //GetKey2ValPairByPos:已知最多存放n个 Key&Value 对,按位置取出第[0,n-1]个Key&Val对
	    int GetKey2ValPairByPos(int iPos,TKEY *&ptKey,TVAL *&ptVal)
	    {
	        ptKey = NULL;
	        ptVal = NULL;

	        if (!m_pstMapDataNodeRoot)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::GetKey2ValPairByPos fail:not init yet...");
	            return  -1;
	        }
	        
	        if (iPos <0 || iPos >= m_iMaxDataNum)
	        {
	            snprintf(m_sErrMsg,255,"CHashMapFixed::GetKey2ValPairByPos fail:Invalid pos<%d>",
	                            iPos);
	            return  -1;
	        }

	        STMapDataNode *pstMapDataNode = m_pstMapDataNodeRoot + iPos;
	        ptKey = &pstMapDataNode->m_data.m_Key;
	        ptVal = &pstMapDataNode->m_data.m_Val;

	        return  0;
	    }

	private:
	    // Map中使用的Key,Val 对
	    struct  STKeyValPair {
	        TKEY    m_Key;          // Key
	        TVAL    m_Val;          // Value
	    };

	    struct STMapDataNode {
	        STKeyValPair    m_data;         // Key&Val pair
	        STMapDataNode   *m_pNext;       // 指向hash list中下一个节点

	        STMapDataNode   *m_pTimeNxt;    // 时间链表后一节点
	        STMapDataNode   *m_pTimePre;    // 时间链表前一节点
	    };
	    typedef    STMapDataNode    *STMapDataNodePtr;

	    int                 m_iIndexNum;    // IndexArray长度
	    STMapDataNodePtr    *m_pstMapDataNodeListHeadPtrs;  // 指向Hash桶对应的list结构的Head

	    int                 m_iMaxDataNum;  // 实际存放数据最大数
	    STMapDataNode       *m_pstMapDataNodeRoot;
	    CObjectMngPool<STMapDataNode>  m_oEntireMapDataNodePool;   // 缓存容器中空闲节点

	    STMapDataNodePtr    m_pstMapDataNodeTimeListHead;   // 时间链表头
	    STMapDataNodePtr    m_pstMapDataNodeTimeListIter;   // 时间链表线形查找的Iterator

	    char                m_sErrMsg[255 + 1];
};


#endif

