#ifndef _SIMP_SHM_20100810_H
#define _SIMP_SHM_20100810_H

#include    <stdio.h>
#include    <unistd.h>
#include    <errno.h>
#include    <string.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/shm.h>
#include    <sys/sem.h>

// 创建一块空闲内存
// pShmRoot:指针引用,当创建/挂接一块共享内存成功时,返回起始地址
// tShmKey:共享内存Key
// iShmSize:共享内存大小
// bCreate:true 若共享内存不存在则自动创建 false挂接到已有得一块内存上
// bNew:若共享内存为新建，bNew等于true
// 返回值:
// < 0  fail
// >=0  Shm-ID
int AttachShm(void **ppShmRoot,key_t tShmKey,int iShmSize,bool bCreate,bool &bNew);

// 解除到共享内存得连接,pShmRoot指向共享内存首地址
int DetachShm(void **ppShmRoot);

char* GetShm(int iKey, int iSize, int iFlag);
int GetShm2(void **pstShm, int iShmID, int iSize, int iFlag);
int GetShm3(void **pstShm, int iShmID, int iSize, int iFlag);


#endif

