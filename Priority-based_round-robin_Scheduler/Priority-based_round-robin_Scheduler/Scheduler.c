#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"

#include <stdio.h>

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

			/* update thread table */
			for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
				if (!pThreadTblEnt[id].bUsed) continue;
				if (pThreadTblEnt[id].pThread->pid == tmp->pid) {
					pThreadTblEnt[id].pThread = tmp;
					break;
				}
			}
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
		pReadyQueueEnt[
			idx].queueCount--;
		return pThread;
	}

	return NULL;
}

/* get highest priority from  ready queue*/
int get_priorityFromReady() {
	for (thread_t id = 0; id < MAX_READYQUEUE_NUM; id++) {
		if (pReadyQueueEnt[id].queueCount) return id;
	}
	return -1;
}

/* make sure ready queue is empty */
BOOL is_empty() {
	for (thread_t id = 0; id < MAX_READYQUEUE_NUM; id++) {
		if (pReadyQueueEnt[id].queueCount) return 0;
	}
	return 1;
}

/* make sure thread table is empty */
BOOL is_threadTbl_empty() {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++)
		if (pThreadTblEnt[id].bUsed) return 0;

	return 1;
}

/* get thread id from thread table */
thread_t get_threadID(const Thread* pThread) {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTblEnt[id].bUsed) continue;
		if (pThreadTblEnt[id].pThread == pThread)
			return id;
	}

	return -1;
}

int RunScheduler(void) {
	/* reset alarm */
	alarm(0);
	if (!is_threadTbl_empty()) {
		for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
			if (!pThreadTblEnt[id].bUsed) continue;
			if (pThreadTblEnt[id].pThread->pid == (int)getpid()) {
				kill(getppid(), SIGUSR1);
				return 0;
			}
		}
	}

	/* prepare scheduling */
	if (pCurrentThread != NULL) 
		kill(pCurrentThread->pid, SIGSTOP);

	/* ready queue is empty */
	if (is_empty()) {
		/* keep running current thread */
		if (pCurrentThread != NULL) {
			pCurrentThread->status = THREAD_STATUS_RUN;
			kill(pCurrentThread->pid, SIGCONT);
		}
		alarm(TIMESLICE);
	}
	else {
		/* when the thread first enter the cpu */
		if (pCurrentThread == NULL) {
			pCurrentThread = GetThreadFromReady();
			pCurrentThread->status = THREAD_STATUS_RUN;
			kill(pCurrentThread->pid, SIGCONT);
			alarm(TIMESLICE);
		}
		else {
			int nPriority = get_priorityFromReady();
			/* no context_switching */
			if (nPriority > pCurrentThread->priority) {
				kill(pCurrentThread->pid, SIGCONT);
				alarm(TIMESLICE);
			}
			/* context_switching */
			else {
				Thread* nThread = (Thread*)malloc(sizeof(Thread));
				nThread = GetThreadFromReady();
				InsertThreadToReady(pCurrentThread);

				thread_t curtid, newtid;
				curtid = get_threadID(pCurrentThread);
				newtid = get_threadID(nThread);
				__ContextSwitch(curtid, newtid);
			}
		}
	}
}

/* context switching */
void __ContextSwitch(int curpid, int newpid){
	/* stop current thread */
	pThreadTblEnt[curpid].pThread->status = THREAD_STATUS_READY;
	//kill(pThreadTblEnt[curpid].pThread->pid, SIGSTOP);

	/* change cpu thread */
	pCurrentThread = pThreadTblEnt[newpid].pThread;

	/* start new thread */
	pThreadTblEnt[newpid].pThread->status = THREAD_STATUS_RUN;
	kill(pThreadTblEnt[newpid].pThread->pid, SIGCONT);
	alarm(TIMESLICE);
}
