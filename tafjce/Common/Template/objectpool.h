#ifndef		_OBJECT_POOL_20100829_H_
#define		_OBJECT_POOL_20100829_H_
#include 	<stdio.h>
#include	<unistd.h>
#include	<string.h>

template <typename ObjectType>

struct STListNodeObject{
	STListNodeObject * m_pstNextNode;
	ObjectType *m_poObject;
	STListNodeObject(): m_pstNextNode( NULL), m_poObject( NULL){}
};

template <typename ObjectType>
class CObjectMngPool
{
	public:
		CObjectMngPool():m_iFreeNodeNum(0), m_pstFreeListHead(NULL), m_pstFreeListTail(NULL),
			m_iObjectNodeNum(0), m_pstObjectListHead(NULL), m_pstObjectListTail(NULL), m_pstListNodeRoot(NULL)
		{}

		~CObjectMngPool(){
			if( m_pstListNodeRoot){
				delete []m_pstListNodeRoot;
				m_pstListNodeRoot = NULL;
			}
		}

		int InitPool( int iCapacity){
			if( NULL != m_pstListNodeRoot){
				return -1;
			}

			m_pstListNodeRoot = new STListNodeObject<ObjectType>[iCapacity];
			if( NULL == m_pstListNodeRoot){
				return -1;
			}

			for( int iIndex =0; iIndex < iCapacity; iIndex ++) 
				Add2FreeList(m_pstListNodeRoot + iIndex );

			 return 0;
		}
		
		int AttachObject2FreeNode( ObjectType *poObj, int iOffset ){
			STListNodeObject<ObjectType> *pFreeListNode = m_pstFreeListHead;
			for( short sIndex =0; (!pFreeListNode ) && (sIndex < iOffset); sIndex ++ ){
				pFreeListNode = pFreeListNode->m_pstNextNode;
			}

			if( NULL != pFreeListNode){
				pFreeListNode->m_poObject = poObj;
				return 0;
			}
			return  -1;
		}
		
		int ObjectBackPush(ObjectType * poObject){
			assert( NULL != poObject);

			STListNodeObject<ObjectType> *pFreeNode = GetFreeListNode();
			if( !pFreeNode){
				snprintf(m_sErrMsg, 255,"There is no free ListNode for push back");
				return -1;
			}

			pFreeNode->m_poObject = poObject;
			Add2ObjectList( pFreeNode);

			return 0;
		}	
				
		ObjectType *ObjectFrontPeek(){
			if( !m_pstObjectListHead)
				return NULL;
		
			return m_pstObjectListHead->m_poObject;
		}
		
		
		ObjectType *ObjectBackPeek(){
			if( !m_pstObjectListTail)
				return NULL;

			return m_pstObjectListTail->m_poObject;
		}
		
		ObjectType *ObjectFrontPop(){
			STListNodeObject<ObjectType> *pObjectNode = GetObjectListNode();
			if( !pObjectNode){
				return NULL;
			}

			ObjectType *pObject = pObjectNode->m_poObject;
			Add2FreeList( pObjectNode);
			
			return  pObject;
			
		}

		int GetObjectNum() const { return m_iObjectNodeNum; }
		int GetFreeCapacity() const { return m_iFreeNodeNum; }

		const char * GetErrMsg() { return m_sErrMsg;}

	private:
		int 	m_iFreeNodeNum;
		STListNodeObject<ObjectType> *m_pstFreeListHead;
		STListNodeObject<ObjectType> *m_pstFreeListTail;

		int 		m_iObjectNodeNum;
		STListNodeObject<ObjectType> *m_pstObjectListHead;
		STListNodeObject<ObjectType> *m_pstObjectListTail;

		STListNodeObject<ObjectType> *m_pstListNodeRoot;
		char m_sErrMsg[256];

		void Add2FreeList(STListNodeObject<ObjectType> *pListNode){
			assert( NULL != pListNode);

			pListNode->m_pstNextNode	= NULL;
			if( ! m_pstFreeListTail ){
				// 第一个节点
				m_pstFreeListHead = m_pstFreeListTail = pListNode;
			}
			else{
				m_pstFreeListTail->m_pstNextNode = pListNode;
				m_pstFreeListTail = pListNode;
			}

			m_iFreeNodeNum ++;
		}

		STListNodeObject<ObjectType> *GetFreeListNode(){
			if( !m_pstFreeListHead)
				return NULL;

			STListNodeObject<ObjectType> *pFreeNode = m_pstFreeListHead;
			m_pstFreeListHead = pFreeNode->m_pstNextNode;
			m_iFreeNodeNum --;

			if(!m_pstFreeListHead )
				m_pstFreeListTail = NULL;

			return pFreeNode;
		}

		void Add2ObjectList( STListNodeObject<ObjectType> *pListNode){
			pListNode->m_pstNextNode = NULL;
			if( !m_pstObjectListTail){
				m_pstObjectListHead = m_pstObjectListTail = pListNode;
			}
			else {
				m_pstObjectListTail->m_pstNextNode = pListNode;
				m_pstObjectListTail	= pListNode;
			}
			m_iObjectNodeNum ++;
		}

		STListNodeObject<ObjectType> *GetObjectListNode(){
			if( !m_pstObjectListHead)
				return NULL;

			STListNodeObject<ObjectType> *pObjectNode = m_pstObjectListHead;
			m_pstObjectListHead	= pObjectNode->m_pstNextNode;
			m_iObjectNodeNum --;

			if( !m_pstObjectListHead)
				m_pstObjectListTail = NULL;

			return pObjectNode;
		}
};
#endif

