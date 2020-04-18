#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdio.h>

void printq() {
	printf("\n");
	printf("\n");
	printf("\n");
	printf("===============THREAD QUEUE=================\n");
	for (int i = 0; i < 5; i++) {
		if (!pThreadTbEnt[i].bUsed)continue;
		printf("%d | pid : %d   priority : %d\n", i, pThreadTbEnt[i].pThread->pid, pThreadTbEnt[i].pThread->priority);
	}
	printf("===============READY QUEUE=================\n");
	for (int i = 0; i < MAX_READYQUEUE_NUM; i++) {
		printf("%d : ", i);
		Thread* temp = pReadyQueueEnt[i].pHead;
		for (int j = 0; j < pReadyQueueEnt[i].queueCount; j++) {
			printf(" %d", temp->pid);
			temp = temp->phNext;
		}
		printf("\n");
	}
	printf("===============CPU QUEUE=================\n");
	printf("%d\n", pCurrentThead->pid);
}

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
	for (thread_t idx = 0; idx < MAX_READYQUEUE_NUM; idx++) {
		if (pReadyQueueEnt[idx].queueCount) return 0;
	}
	return 1;
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

int RunScheduler(void) {
	printq();
	signal(SIGALRM, RunScheduler);
	/*if (pCurrentThead != NULL) {
		if (!is_empty()) {
			InsertThreadToReady(pCurrentThead);
			Thread* nThread = (Thread*)malloc(sizeof(Thread));
			nThread = GetThreadFromReady();
			thread_t tid1, tid2;
			tid1 = get_threadID(pCurrentThead);
			tid2 = get_threadID(nThread);
			__ContextSwitch(tid1, tid2);
		}
	}
	else {
		pCurrentThead = GetThreadFromReady();
		pCurrentThead->status = THREAD_STATUS_RUN;
		kill(pCurrentThead->pid, SIGCONT);
	}*/
	if (is_empty()) {
		kill(pCurrentThead->pid, SIGCONT);
	}
	else {
		InsertThreadToReady(pCurrentThead);
		Thread* nThread = (Thread*)malloc(sizeof(Thread));
		nThread = GetThreadFromReady();
		thread_t tid1, tid2;
		tid1 = get_threadID(pCurrentThead);
		tid2 = get_threadID(nThread);
		__ContextSwitch(tid1, tid2);
	}

	alarm(TIMESLICE);
}

/* context switching */
void __ContextSwitch(int curpid, int newpid){
	printf("context switching : %d >> %d\n", curpid, newpid);
	printf("cur pid : %d\n", getpid());
	Thread* pThread = (Thread*)malloc(sizeof(Thread));

	/* start new thread */
	pThread = pThreadTbEnt[newpid].pThread;
	printf("start pid : %d\n", pThread->pid);
	kill(pThread->pid, SIGCONT);
	pThread->status = THREAD_STATUS_RUN;

	printf("change cpu\n");
	/* change cpu thread */
	pCurrentThead = pThread;

	/* stop current thread */
	pThread = pThreadTbEnt[curpid].pThread;
	printf("kill pid : %d\n", pThread->pid);
	kill(pThread->pid, SIGSTOP);
	pThread->status = THREAD_STATUS_READY;
}
