
#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#define STACK_SIZE    1024*64
#define CLONE_FS      0x00000200
#define CLONE_FILES   0x00000400
#define CLONE_SIGHAND 0x00000800
#define CLONE_VM      0x00000100

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

/* get thread id from thread table */
int get_thread_id(Thread* pThread) {
	for (int id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTbEnt[id].bUsed) {
			pThreadTbEnt[id].bUsed = 1;
			pThreadTbEnt[id].pThread = pThread;
			return id;
		}
	}
}

/* scheduling 추가 구현하기 */
int thread_create(thread_t *thread, thread_attr_t *attr, int priority, void *(*start_routine) (void *), void *arg){
	void* stack = malloc(STACK_SIZE);
	if (!stack) {
		perror("malloc error");
		exit(1);
	}

	/* create thread */
	int flags = SIGCHLD|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_VM;
	pid_t pid = clone(*start_routine, (char*)stack + STACK_SIZE, flags, &arg);

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
	/* go readt queue */
	else {
		pThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pThread, priority);
	}

	return 1;
}

int thread_suspend(thread_t tid){

}

int thread_cancel(thread_t tid){

}

int thread_resume(thread_t tid){

}

thread_t thread_self(){

}

/*
void func(void *arg) {
	printf("run\n");
	return;
}

void main(void) {
	int pid;
	int arg = 100;
	int flags = SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM;
	char* pStack;
	pStack = malloc(STACK_SIZE);

	pid = clone(func, (char*)pStack + STACK_SIZE, flags, &arg);
	waitpid(pid, NULL);

	return;
}
*/