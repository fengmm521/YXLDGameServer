#include    "simple_shm.h"

int AttachShm(void **ppShmRoot,key_t tShmKey,int iShmSize,bool bCreate,bool &bNew){
    int    iShmId = -1;
    *ppShmRoot = NULL;

    if (bCreate){
        iShmId = shmget(tShmKey,iShmSize,IPC_CREAT|IPC_EXCL|0666);
		
        if (iShmId < 0){
            if (errno != EEXIST)
                return    -1;

            bNew = false;
        }
        else{
            bNew = true;
        }
    }

    if (iShmId < 0){
        iShmId = shmget(tShmKey,iShmSize,0666);
        if (iShmId < 0)
            return    -2;
    }

    *ppShmRoot = shmat(iShmId,NULL,0);
    if (*ppShmRoot == NULL || (*ppShmRoot ==((void*) -1)))
        return    -3;

    return    iShmId;
}

int DetachShm(void **ppShmRoot){
    if (!(*ppShmRoot))
        return    -1;

    shmdt(*ppShmRoot);
    *ppShmRoot = NULL;

    return    0;
}

char* GetShm(int iKey, int iSize, int iFlag){
	int iShmID;
	char* sShm;
	char sErrMsg[50];

	if ((iShmID = shmget(iKey, iSize, iFlag)) < 0) {
		sprintf(sErrMsg, "shmget %d %d", iKey, iSize);
		perror(sErrMsg);
		return NULL;
	}
	
	if ((sShm = (char*)shmat(iShmID, NULL ,0)) == (char *) -1) {
		perror("shmat");
		return NULL;
	}
	return sShm;
}

int GetShm2(void **pstShm, int iShmID, int iSize, int iFlag){
	char* sShm;

	if (!(sShm = GetShm(iShmID, iSize, iFlag & (~IPC_CREAT)))) {
		if (!(iFlag & IPC_CREAT)) return -1;
		if (!(sShm = GetShm(iShmID, iSize, iFlag))) return -1;
		memset(sShm, 0, iSize);
	}
	
	*pstShm = sShm;
	return 0;
}

int GetShm3(void **pstShm, int iShmID, int iSize, int iFlag){
	char* sShm;

	if (!(sShm = GetShm(iShmID, iSize, iFlag & (~IPC_CREAT)))) {
		if (!(iFlag & IPC_CREAT)) return -1;
		if (!(sShm = GetShm(iShmID, iSize, iFlag))) return -1;
		
		*pstShm = sShm;
		return 1;
	}
	*pstShm = sShm;
	return 0;
}


