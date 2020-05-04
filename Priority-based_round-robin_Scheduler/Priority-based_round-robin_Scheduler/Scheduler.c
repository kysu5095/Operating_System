#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdio.h>

void printq() {
	printf("%d : \n", getpid());
	printf("===============pTHREAD ENT================\n");
	for (thread_t i = 0; i < MAX_THREAD_NUM; i++) {
		if (!pThreadTbEnt[i].bUsed)continue;
		printf("%d | pid : %d(%p)    priority : %d\n", i, pThreadTbEnt[i].pThread->pid, pThreadTbEnt[i].pThread, pThreadTbEnt[i].pThread->priority);
	}
	printf("===============READY QUEUE=================\n");
	for (int i = 0; i < MAX_READYQUEUE_NUM; i++) {
		if (!pReadyQueueEnt[i].queueCount)continue;
		printf("%d |", i);
		Thread* temp = (Thread*)malloc(sizeof(Thread));
		temp = pReadyQueueEnt[i].pHead;
		for (int j = 0; j < pReadyQueueEnt[i].queueCount; j++) {
			printf(" %d(%p)", temp->pid, temp);
			temp = temp->phNext;
		}
		printf("\n");
	}
	printf("===============WAITING QUEUE=================\n");
	Thread* temp = (Thread*)malloc(sizeof(Thread));
	temp = pWaitingQueueHead->phNext;
	while (temp->phNext != NULL) {
		printf("%d(%p)\n", temp->pid, temp);
		temp = temp->phNext;
	}
	printf("\n");
	printf("================CPU QUEUE==================\n");
	if (pCurrentThead != NULL)
		printf("%d(%p)\n", pCurrentThead->pid, pCurrentThead);
	else
		printf("cpu empty\n");
	printf("=================EXE PID===================\n");
	printf("%d\n", getpid());
	printf("\n");
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

			/* update thread table */
			for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
				if (!pThreadTbEnt[id].bUsed) continue;
				if (pThreadTbEnt[id].pThread->pid == tmp->pid) {
					pThreadTbEnt[id].pThread = tmp;
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

/* get thread id from thread table */
thread_t get_threadID(const Thread* pThread) {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTbEnt[id].bUsed) continue;
		if (pThreadTbEnt[id].pThread == pThread)
			return id;
	}

	return -1;
}

void RunScheduler(void) {
	//printf("%d : run scheduler\n", getpid());
	/* reset alarm */
	alarm(0);
	//printf("=============BEFORE SCHEDULER==============\n");
	////printq();;
	/* prepare scheduling */
	if (pCurrentThead != NULL) 
		kill(pCurrentThead->pid, SIGSTOP);

	/* ready queue is empty */
	if (is_empty()) {
		/* keep running current thread */
		if (pCurrentThead != NULL) {
			pCurrentThead->status = THREAD_STATUS_RUN;
			kill(pCurrentThead->pid, SIGCONT);
		}
		//printf("=============AFTER SCHEDULER==============\n");
		//printq();;
		alarm(TIMESLICE);
	}
	else {
		/* when the thread first enter the cpu */
		if (pCurrentThead == NULL) {
			pCurrentThead = GetThreadFromReady();
			pCurrentThead->status = THREAD_STATUS_RUN;
			kill(pCurrentThead->pid, SIGCONT);
			//printf("=============AFTER SCHEDULER==============\n");
			//printq();;
			alarm(TIMESLICE);
		}
		else {
			int nPriority = get_priorityFromReady();
			/* no context_switching */
			if (nPriority > pCurrentThead->priority) {
				kill(pCurrentThead->pid, SIGCONT);
				//printf("=============AFTER SCHEDULER==============\n");
				//printq();;
				alarm(TIMESLICE);
			}
			/* context_switching */
			else {
				Thread* nThread = (Thread*)malloc(sizeof(Thread));
				nThread = GetThreadFromReady();
				InsertThreadToReady(pCurrentThead);

				thread_t curtid, newtid;
				curtid = get_threadID(pCurrentThead);
				newtid = get_threadID(nThread);
				__ContextSwitch(curtid, newtid);
			}
		}
	}
}

/* context switching */
void __ContextSwitch(int curpid, int newpid){
	//printf("%d : context switch %d -> %d\n", getpid(), curpid, newpid);
	/* stop current thread */
	pThreadTbEnt[curpid].pThread->status = THREAD_STATUS_READY;
	//kill(pThreadTbEnt[curpid].pThread->pid, SIGSTOP);

	/* change cpu thread */
	pCurrentThead = pThreadTbEnt[newpid].pThread;

	/* start new thread */
	pThreadTbEnt[newpid].pThread->status = THREAD_STATUS_RUN;
	kill(pThreadTbEnt[newpid].pThread->pid, SIGCONT);
	//printf("=============AFTER SCHEDULER==============\n");
	//printq();;
	alarm(TIMESLICE);
}
