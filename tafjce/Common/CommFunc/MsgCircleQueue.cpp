#include "define.h"
#include "simple_shm.h"
#include "MsgCircleQueue.h"
#define  PUSHPOS_OFFSET     0x0
#define  POPPOS_OFFSET      0x10
#define  MSGBUF_OFFSET      0x20

CMsgCircleQueue::CMsgCircleQueue():m_pShmRoot(NULL), m_pMsgBuf(NULL), m_pPushOffset(NULL), 
	m_pPopOffset(NULL), m_dwBufSize(0)

{
}

CMsgCircleQueue::~CMsgCircleQueue()
{
	if(m_pShmRoot)
	{
		delete m_pShmRoot;
		m_pShmRoot = NULL;
	}
}


bool CMsgCircleQueue::Create(DWORD dwShmSize){
   
	m_pShmRoot = new char[dwShmSize];
    memset( m_pShmRoot, 0, dwShmSize);

    m_pMsgBuf       = m_pShmRoot + MSGBUF_OFFSET;
    m_pPushOffset   = m_pShmRoot + PUSHPOS_OFFSET;
    m_pPopOffset    = m_pShmRoot + POPPOS_OFFSET;
    m_dwBufSize     = dwShmSize - MSGBUF_OFFSET;    // 实际消息BUF大小

    return true;
}


DWORD   CMsgCircleQueue::PushMsg(char *pBuf, DWORD dwBufSize){
	// 防止无法判断满和空(挖掉一个字节)
	if( GetFreeSize() <= dwBufSize)
        return 0;

    DWORD dwCopySize    = std::min(dwBufSize, GetShmMsgBufSize() - GetPushOffset());
    memcpy( GetPushBuf(), pBuf, dwCopySize);
    memcpy( m_pMsgBuf, &pBuf[dwCopySize], dwBufSize - dwCopySize);
    
    SetPushOffset( (GetPushOffset() + dwBufSize) % GetShmMsgBufSize() );
    return dwBufSize;
}

void  CMsgCircleQueue::RefreshPopOffset(DWORD dwSize){
    SetPopOffset( (GetPopOffset() + dwSize) % GetShmMsgBufSize());
}

void  CMsgCircleQueue::PopConnPos(INT64 &iConnPos){

    DWORD dwCopySize    = std::min((DWORD)sizeof(INT64), GetShmMsgBufSize() - GetPopOffset());
    memcpy( m_aConnPos, GetPopBuf(), dwCopySize );
    memcpy( &m_aConnPos[dwCopySize], m_pMsgBuf, sizeof(INT64) - dwCopySize );
    
    iConnPos = *(INT64*)(m_aConnPos);
    RefreshPopOffset( sizeof(INT64));
}

void  CMsgCircleQueue::PopMsgLen(WORD &wMsgLen){
    
    DWORD dwCopySize    = std::min( (DWORD)sizeof(WORD), GetShmMsgBufSize() - GetPopOffset());
    memcpy( m_aMsgSize, GetPopBuf(), dwCopySize );
    memcpy( &m_aMsgSize[dwCopySize], m_pMsgBuf, sizeof(WORD) - dwCopySize );
    
    wMsgLen =*(WORD*)(m_aMsgSize);
    RefreshPopOffset( sizeof(WORD));
}

void  CMsgCircleQueue::PopRsObjAddr( string &sRsObjAddr){
    WORD wRsObjAddLen;
    PopMsgLen( wRsObjAddLen );

	sRsObjAddr="";
    DWORD dwCopySize    = std::min( (DWORD)wRsObjAddLen, GetShmMsgBufSize() - GetPopOffset());

	sRsObjAddr.append(GetPopBuf(), dwCopySize);
	sRsObjAddr.append(m_pMsgBuf, wRsObjAddLen - dwCopySize);
	    
    RefreshPopOffset(wRsObjAddLen );
}

bool  CMsgCircleQueue::PopMsg(char *pSendBuf,WORD &wSendSize,INT64 &iConnPos, string &sRsObjAddr){
	if(IsEmpty() ) return false;
    
    PopRsObjAddr(sRsObjAddr);
	PopConnPos(iConnPos);
    PopMsgLen( wSendSize );

    DWORD dwCopySize    = std::min( (DWORD)wSendSize, GetShmMsgBufSize() - GetPopOffset());
    memcpy( pSendBuf, GetPopBuf(), dwCopySize );
    memcpy( &pSendBuf[dwCopySize], m_pMsgBuf, wSendSize - dwCopySize );
    
    RefreshPopOffset( wSendSize);
	return true;
}

DWORD CMsgCircleQueue::GetFreeSize(){
    if (GetPushOffset() < GetPopOffset())
        return GetPopOffset() - GetPushOffset();
    else
        return GetShmMsgBufSize() - GetPushOffset() + GetPopOffset();
}


