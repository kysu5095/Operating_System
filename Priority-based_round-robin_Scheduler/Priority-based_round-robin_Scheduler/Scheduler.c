#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdlib.h>

/* insert thread to **Tail** of ready queue */
void InsertThreadToReady(Thread* pThread) {
	int priority = pThread->priority;
	// when ready queue empty
	if (pReadyQueueEnt[priority].pHead == NULL) {
		pReadyQueueEnt[priority].pHead         = pThread;
		pReadyQueueEnt[priority].pHead->phPrev = NULL;
		pReadyQueueEnt[priority].pHead->phNext = NULL;
	}
	else {
		// when ready queue have only head
		if (pReadyQueueEnt[priority].pTail == NULL) {
			pReadyQueueEnt[priority].pTail         = pThread;
			pReadyQueueEnt[priority].pTail->phNext = NULL;
			pReadyQueueEnt[priority].pTail->phPrev = pReadyQueueEnt[priority].pHead;
			pReadyQueueEnt[priority].pHead->phNext = pReadyQueueEnt[priority].pTail;
		}
		else {
			Thread* tmp = (Thread*)malloc(sizeof(Thread));
			tmp->stackSize = pReadyQueueEnt[priority].pTail->stackSize;
			tmp->stackAddr = pReadyQueueEnt[priority].pTail->stackAddr;
			tmp->status    = pReadyQueueEnt[priority].pTail->status;
			tmp->exitCode  = pReadyQueueEnt[priority].pTail->exitCode;
			tmp->pid       = pReadyQueueEnt[priority].pTail->pid;
			tmp->priority  = pReadyQueueEnt[priority].pTail->priority;
			tmp->phPrev    = pReadyQueueEnt[priority].pTail->phPrev;
			pReadyQueueEnt[priority].pTail->phPrev->phNext = tmp;
			pReadyQueueEnt[priority].pTail                 = pThread;
			pReadyQueueEnt[priority].pTail->phPrev         = tmp;
			pReadyQueueEnt[priority].pTail->phNext         = NULL;
			tmp->phNext = pReadyQueueEnt[priority].pTail;
		}
	}
	pReadyQueueEnt[priority].queueCount++;
}

/* delete thread from ready queue */
Thread* GetThreadFromReady() {
	Thread* pThread = (Thread*)malloc(sizeof(Thread));
	for (thread_t idx = 0; idx < MAX_READYQUEUE_NUM; idx++) {
		if (!pReadyQueueEnt[idx].queueCount) continue;
		pThread = pReadyQueueEnt[idx].pHead;
		/* when ready queue size is 1 */
		if (pThread->phNext == NULL) {
			pReadyQueueEnt[idx].pHead = NULL;
			pReadyQueueEnt[idx].pTail = NULL;
		}
		else {
			/* when ready queue size is 2 */
			if (pThread->phNext == pReadyQueueEnt[idx].pTail) {
				pReadyQueueEnt[idx].pHead = pReadyQueueEnt[idx].pTail;
				pReadyQueueEnt[idx].pHead->phNext = NULL;
				pReadyQueueEnt[idx].pHead->phPrev = NULL;
				pReadyQueueEnt[idx].pTail = NULL;
			}
			else {
				pReadyQueueEnt[idx].pHead = pReadyQueueEnt[idx].pHead->phNext;
				pReadyQueueEnt[idx].pHead->phPrev = NULL;
			}
		}
		pReadyQueueEnt[idx].queueCount--;
		return pThread;
	}

	return NULL;
}

/* make sure ready queue is empty */
BOOL is_empty() {
	for (int idx = 0; idx < MAX_READYQUEUE_NUM; idx++) {
		if (pReadyQueueEnt[idx].queueCount) return 1;
	}
	return 0;
}

/* get thread id from thread table */
thread_t get_threadID(Thread* pThread) {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTbEnt[id].bUsed) continue;
		if (pThreadTbEnt[id].pThread == pThread)
			return id;
	}

	return -1;
}

int RunScheduler( void ){
	if (is_empty()) {
		InsertThreadToReady(pCurrentThead);
		Thread* nThread = (Thread*)malloc(sizeof(Thread));
		nThread = GetThreadFromReady();
		thread_t tid1, tid2;
		tid1 = get_threadID(pCurrentThead);
		tid2 = get_threadID(nThread);
		__ContextSwitch(tid1, tid2);
		////////////////////////////
		////////////////////////////
		// alarm Ãß°¡
		////////////////////////////
		////////////////////////////
	}
}

/* context switching */
void __ContextSwitch(int curpid, int newpid){
	Thread* pThread = (Thread*)malloc(sizeof(Thread));

	/* stop current thread */
	pThread = pThreadTbEnt[curpid].pThread;
	kill(pThread->pid, SIGSTOP);
	pThread->status = THREAD_STATUS_READY;
	
	/* start new thread */
	pThread = pThreadTbEnt[newpid].pThread;
	kill(pThread->pid, SIGCONT);
	pThread->status = THREAD_STATUS_RUN;

	/* change cpu thread */
	pCurrentThead = pThread;
}
