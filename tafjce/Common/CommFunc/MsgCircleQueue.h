#ifndef _MSG_CIRCLE_QUEUE_20110505_H_
#define _MSG_CIRCLE_QUEUE_20110505_H_

using namespace std;

class CMsgCircleQueue{
    public:
        CMsgCircleQueue();
        ~CMsgCircleQueue();
        bool    Create(DWORD dwShmSize);
        DWORD   PushMsg(char *pBuf, DWORD dwBufSize);
        bool    PopMsg(char *pSendBuf,WORD &wSendSize,INT64 &iConnPos, string &sRsObjAddr);
    private:
        void    PopConnPos(INT64 &iConnPos);
        void    PopMsgLen(WORD &wMsgLen);
        void    PopRsObjAddr( string &sRsObjAddr);
        void    RefreshPopOffset(DWORD dwSize);
                
        char *  GetPushBuf(){ return &m_pMsgBuf[GetPushOffset()];}
        char *  GetPopBuf(){  return &m_pMsgBuf[GetPopOffset()]; }
        bool    IsEmpty(){ return (GetPopOffset() == GetPushOffset());}
        DWORD   GetFreeSize();
        void    SetPushOffset(DWORD dwPushOffset){    *(DWORD*)m_pPushOffset  = dwPushOffset;}
        void    SetPopOffset( DWORD dwPopOffset){  *(DWORD*)m_pPopOffset   = dwPopOffset;}
        DWORD   GetPushOffset(){ return *(DWORD*) m_pPushOffset;}
        DWORD   GetPopOffset(){  return *(DWORD*) m_pPopOffset; }
        DWORD   GetShmMsgBufSize(){ return m_dwBufSize;}
        //DWORD   MIN( DWORD dwVal1, DWORD dwVal2){ return (dwVal1> dwVal2) ? dwVal2 : dwVal1;}
    private:
        char *  m_pShmRoot;         // 整个Shm的基地址
        char *  m_pMsgBuf;          // 消息部分的基地址
        char *  m_pPushOffset;      // 写缓冲的位置偏移
        char*   m_pPopOffset;       // 读缓冲的位置偏移
        DWORD   m_dwBufSize;
        char    m_aConnPos[sizeof(INT64)];
        char    m_aMsgSize[sizeof(short)];
};
#endif

