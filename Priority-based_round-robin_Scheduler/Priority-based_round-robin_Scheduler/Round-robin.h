#ifndef __HW1_H__
#define __HW1_H__

#include <stdio.h>
#include <unistd.h>

#define HASH_TBL_SIZE    (8)
#define MAX_OBJLIST_NUM  (3)
#define OBJ_INVALID  (-1)

typedef struct Thread Thread;
typedef int BOOL;

enum {
	THREAD_STATUS_RUN    = 0,
	THREAD_STATUS_READY  = 1,
	THREAD_STATUS_WAIT   = 2,
	THREAD_STATUS_ZOMBIE = 3,
}ThreadStatus;

struct Thread
{
	int			  stackSize;
	void*	      stackAddr;
	ThreadStatus  status;
	int           exitCode;
	pid_t         pid;
	int           objnum;
	Thread*       phPrev;
	Thread*       phNext;
};

typedef struct _ReadyQueueEnt
{
	int queueCount;
	Thread*  pHead;
	Thread*  pTail;
} ReadyQueueEnt;

ReadyQueueEnt pReadyQueueEnt[HASH_TBL_SIZE];


Thread*  pWatingQueueHead;
Thread*  pWaitingQueueTail;


void Init();
void InsertThreadToTail(Thread* pObj, int ObjNum);
void InsertThreadToHead(Thread* pObj, int objNum);
Thread* GetThreadByNum(int objnum);
Thread* GetThreadFromObjFreeList();
BOOL DeleteThread(Thread* pObj);
void InsertThreadIntoObjFreeList(Thread* pObj);

#endif
