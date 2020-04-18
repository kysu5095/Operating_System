#define _GNU_SOURCE
#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#define STACK_SIZE    1024*64
//#define CLONE_FS      0x00000200
//#define CLONE_FILES   0x00000400
//#define CLONE_SIGHAND 0x00000800
//#define CLONE_VM      0x00000100

/*
 InsertThreadToTail      : ready queue의 우선순위에 맞는 테이블에 thread를 'tail'로 넣음    
 DeleteThreadFromReady   : ready queue에서 해당하는 thread 삭제                           
 InsertThreadIntoWaiting : waiting queue에 thread를 'tail'로 넣음                         
 DeleteThreadFromWaiting : waiting queue에 해당하는 thread 삭제                           
 get_thread_id           : thread table에서 thread의 index를 반환                         
 thread_create           : thread 생성 후 ready queue로 삽입 or context scheduling        
 thread_suspend          : 해당하는 thread 정지 >> waiting queue로 보냄                    
 thread_cancel           : 해당하는 thread 죽임 >> 메모리 해제 후 thread table에서도 삭제  
 thread_rsume            : 해당하는 thread 다시 시작 >> ready queue or context scheduling  
 thread_self             : 이 함수를 호출한 thread의 tid를 반환                            
*/

/* insert thread to **Tail** of ready queue */
void InsertThreadToTail(Thread* pThread, int priority) {
	// when ready queue empty
	if (pReadyQueueEnt[priority].pHead == NULL) {
		pReadyQueueEnt[priority].pHead = pThread;
		pReadyQueueEnt[priority].pHead->phPrev = NULL;
		pReadyQueueEnt[priority].pHead->phNext = NULL;
	}
	else {
		// when ready queue have only head
		if (pReadyQueueEnt[priority].pTail == NULL) {
			pReadyQueueEnt[priority].pTail = pThread;
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
			pReadyQueueEnt[priority].pTail = pThread;
			pReadyQueueEnt[priority].pTail->phPrev = tmp;
			pReadyQueueEnt[priority].pTail->phNext = NULL;
			tmp->phNext = pReadyQueueEnt[priority].pTail;
		}
	}
	pReadyQueueEnt[priority].queueCount++;
}

/* delete thread from ready queue */
BOOL DeleteThreadFromReady(Thread* pThread) {
	int priority = pThread->priority;
	Thread *tmp = (Thread*)malloc(sizeof(Thread));
	tmp = pReadyQueueEnt[priority].pHead;
	while (tmp != NULL) {
		if (tmp == pThread) {
			// When delete head
			if (tmp == pReadyQueueEnt[priority].pHead) {
				// When hash table size is 1
				if (tmp->phNext == NULL) {
					pReadyQueueEnt[priority].pHead = NULL;
					pReadyQueueEnt[priority].pTail = NULL;
				}
				else {
					// When hash table size is 2
					if (tmp->phNext == pReadyQueueEnt[priority].pTail) {
						pReadyQueueEnt[priority].pHead = pReadyQueueEnt[priority].pTail;
						pReadyQueueEnt[priority].pHead->phNext = NULL;
						pReadyQueueEnt[priority].pHead->phPrev = NULL;
						pReadyQueueEnt[priority].pTail = NULL;
					}
					else {
						pReadyQueueEnt[priority].pHead = pReadyQueueEnt[priority].pHead->phNext;
						pReadyQueueEnt[priority].pHead->phPrev = NULL;
					}
				}
			}
			else {
				// When delete tail
				if (tmp == pReadyQueueEnt[priority].pTail) {
					// When hash table size is 2
					if (tmp->phPrev == pReadyQueueEnt[priority].pHead) {
						pReadyQueueEnt[priority].pHead->phNext = NULL;
						pReadyQueueEnt[priority].pTail = NULL;
					}
					else {
						pReadyQueueEnt[priority].pTail = pReadyQueueEnt[priority].pTail->phPrev;
						pReadyQueueEnt[priority].pTail->phNext = NULL;
					}
				}
				else {
					tmp->phPrev->phNext = tmp->phNext;
					tmp->phNext->phPrev = tmp->phPrev;
				}
			}
			pReadyQueueEnt[priority].queueCount--;
			return 1;
		}
		if (tmp->phNext == NULL) break;
		tmp = tmp->phNext;
	}
	return 0;
}

/* insert thread to **tail** of waiting queue */
void InsertThreadIntoWaiting(Thread* pThread) {
	pWaitingQueueTail->phPrev->phNext = pThread;
	pThread->phPrev = pWaitingQueueTail->phPrev;
	pThread->phNext = pWaitingQueueTail;
	pWaitingQueueTail->phPrev = pThread;
}

/* delete thread form waiting queue */
BOOL DeleteThreadFromWaiting(Thread* pThread) {
	if (pWaitingQueueTail->phPrev == pWaitingQueueHead) 
		return 0;

	Thread* tmp = pWaitingQueueHead->phNext;
	while (1) {
		if (tmp == pThread) {
			tmp->phPrev->phNext = tmp->phNext;
			tmp->phNext->phPrev = tmp->phPrev;
			return 1;
		}
		if (tmp->phNext == pWaitingQueueTail) break;
		tmp = tmp->phNext;
	}
	return 0;
}

/* get thread id from thread table */
thread_t get_thread_id(Thread* pThread) {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTbEnt[id].bUsed) {
			pThreadTbEnt[id].bUsed = 1;
			pThreadTbEnt[id].pThread = pThread;
			return id;
		}
	}
}

/* scheduling 추가 구현하기 */
int thread_create(thread_t *thread, thread_attr_t *attr, int priority, void *(*start_routine) (void *), void *arg){
	void* stack;
	stack = malloc(STACK_SIZE);
	if (!stack) {
		perror("malloc error");
		exit(1);
	}

	/* create thread */
	int flags = SIGCHLD|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_VM;
	pid_t pid = clone((void*)start_routine, (char*)stack + STACK_SIZE, flags, &arg);

	/* stop thread immediately */
	kill(pid, SIGSTOP);

	/* allocate TCB && init TCB */
	Thread* pThread    = (Thread*)malloc(sizeof(Thread));
	pThread->stackSize = STACK_SIZE;
	pThread->stackAddr = *start_routine;
	pThread->exitCode  = 0;
	pThread->pid       = pid;
	pThread->priority  = priority;
	*thread            = get_thread_id(pThread);

	/* context switching */
	if (pCurrentThead == NULL || priority < pCurrentThead->priority) {
		pThread->status = THREAD_STATUS_RUN;
		///////////////////////////
		///////////////////////////
		//  scheduling 작업 추가  //
		///////////////////////////
		///////////////////////////
	}
	/* insert ready queue */
	else {
		pThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pThread, priority);
	}

	return 1;
}

/* suspend thread */
/* note : don't suspend oneself!!!! */
int thread_suspend(thread_t tid){
	/* no such thread */
	if (!pThreadTbEnt[tid].bUsed) return -1;
	/* thread status is not ready */
	if (pThreadTbEnt[tid].pThread->status != THREAD_STATUS_READY) return -1;
	if (DeleteThreadFromReady(pThreadTbEnt[tid].pThread)) {
		pThreadTbEnt[tid].pThread->status = THREAD_STATUS_WAIT;
		InsertThreadIntoWaiting(pThreadTbEnt[tid].pThread);
		return 0;
	}
	else
		return -1;
}
 
/* kill thread */
/* note : don't kill oneself!!!! */
int thread_cancel(thread_t tid){
	pid_t pid = pThreadTbEnt[tid].pThread->pid;
	/* kill thread */
	kill(pid, SIGKILL);
	if (pThreadTbEnt[tid].pThread->status == THREAD_STATUS_READY)
		DeleteThreadFromReady(pThreadTbEnt[tid].pThread);
	if (pThreadTbEnt[tid].pThread->status == THREAD_STATUS_WAIT)
		DeleteThreadFromWating(pThreadTbEnt[tid].pThread);
	pThreadTbEnt[tid].pThread->status = THREAD_STATUS_ZOMBIE;
	
	free(pThreadTbEnt[tid].pThread);
	pThreadTbEnt[tid].bUsed = 0;
	pThreadTbEnt[tid].pThread = NULL;

	return 0;
}

/* resume thread */
int thread_resume(thread_t tid){
	Thread* pThread = (Thread*)malloc(sizeof(Thread));
	pThread = pThreadTbEnt[tid].pThread;

	/* context switching */
	if (pThread->priority < pCurrentThead->priority) {
		pThread->status = THREAD_STATUS_RUN;
		///////////////////////////
		///////////////////////////
		//  scheduling 작업 추가  //
		///////////////////////////
		///////////////////////////
		return 0;
	}
	/* insert ready queue */
	else {
		pThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pThread, pThread->priority);
		return 0;
	}

	return -1;
}

/* get thread id from thread table */
thread_t thread_self(){
	pid_t pid = getpid();
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTbEnt[id].bUsed) continue;
		if (pThreadTbEnt[id].pThread->pid == pid)
			return id;
	}

	return -1;
}


//int func(void *arg) {
//	printf("run\n");
//	return 1;
//}
//
//void main(void) {
//	pid_t pid;
//	int arg = 100;
//	int flags = SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM;
//	void* pStack;
//	pStack = malloc(STACK_SIZE);
//
//	pid = clone(&func, (char*)pStack + STACK_SIZE, flags, &arg);
//	return;
//}
