#define _GNU_SOURCE
#define STACK_SIZE    1024*64
#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdio.h>
#include <stdlib.h>
//#define CLONE_FS      0x00000200
//#define CLONE_FILES   0x00000400
//#define CLONE_SIGHAND 0x00000800
//#define CLONE_VM      0x00000100

/*
 InsertThreadToTail      : ready queue�� �켱������ �´� ���̺� thread�� 'tail'�� ����    
 DeleteThreadFromReady   : ready queue���� �ش��ϴ� thread ����                           
 InsertThreadIntoWaiting : waiting queue�� thread�� 'tail'�� ����                         
 DeleteThreadFromWaiting : waiting queue�� �ش��ϴ� thread ����                           
 set_threadID            : thread table���� thread�� index�� ��ȯ                         
 thread_create           : thread ���� �� ready queue�� ���� or context scheduling        
 thread_suspend          : �ش��ϴ� thread ���� >> waiting queue�� ����                    
 thread_cancel           : �ش��ϴ� thread ���� >> �޸� ���� �� thread table������ ����  
 thread_rsume            : �ش��ϴ� thread �ٽ� ���� >> ready queue or context scheduling  
 thread_self             : �� �Լ��� ȣ���� thread�� tid�� ��ȯ     
 thread_join			 : tid�� �����尡 ������ ������ ��ٸ�
 thread_exit			 : �ڱ� �ڽ��� �����ϴ� �Լ�
*/

/* insert thread to **Tail** of ready queue */
void InsertThreadToTail(Thread* pThread) {
	int priority = pThread->priority;
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
		return -1;

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
thread_t set_threadID(Thread* pThread) {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (!pThreadTbEnt[id].bUsed) {
			pThreadTbEnt[id].bUsed = 1;
			pThreadTbEnt[id].pThread = pThread;
			return id;
		}
	}
}

void exit_signal(int signo) {
	printf("%d : signal arrived %d\n", getpid(), signo);
	kill(getpid(), SIGCONT);
	printf("%d: arrive myselfffffffffffff\n", getpid());
	//signal(SIGCHLD, (void*)exit_signal);
}

/* create thread */
int thread_create(thread_t *thread, thread_attr_t *attr, int priority, void *(*start_routine) (void *), void *arg){
	void* stack;
	stack = malloc(STACK_SIZE);
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
	pThread->phNext    = NULL;
	pThread->phPrev    = NULL;
	thread_t tid	   = set_threadID(pThread);
	*thread			   = tid;

	/* context switching */
	if (pCurrentThead != NULL && priority < pCurrentThead->priority) {
		pCurrentThead->status = THREAD_STATUS_READY;
		InsertThreadToTail(pCurrentThead);
		pThread->status = THREAD_STATUS_RUN;
		kill(getppid(), SIGUSR1);
	}
	/* move TCB to ready queue */
	else{
		pThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pThread);
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
	if (!pThreadTbEnt[tid].bUsed) return -1;
	pid_t pid = pThreadTbEnt[tid].pThread->pid;
	/* kill thread */
	kill(pid, SIGKILL);
	if (pThreadTbEnt[tid].pThread->status == THREAD_STATUS_READY)
		DeleteThreadFromReady(pThreadTbEnt[tid].pThread);
	if (pThreadTbEnt[tid].pThread->status == THREAD_STATUS_WAIT)
		DeleteThreadFromWaiting(pThreadTbEnt[tid].pThread);
	pThreadTbEnt[tid].pThread->status = THREAD_STATUS_ZOMBIE;
	
	free(pThreadTbEnt[tid].pThread);
	pThreadTbEnt[tid].bUsed = 0;
	pThreadTbEnt[tid].pThread = NULL;

	return 0;
}

/* resume thread */
int thread_resume(thread_t tid){
	if (!pThreadTbEnt[tid].bUsed) return -1;
	Thread* pThread = (Thread*)malloc(sizeof(Thread));
	pThread = pThreadTbEnt[tid].pThread;

	/* get thread from waiting queue */
	if (DeleteThreadFromWaiting(pThread) == -1) return -1;

	if (pCurrentThead != NULL && pThread->priority < pCurrentThead->priority) {
		//pCurrentThead->status = THREAD_STATUS_READY;
		//InsertThreadToTail(pCurrentThead);
		//pThread->status = THREAD_STATUS_RUN;
		InsertThreadToTail(pThread);
		kill(getppid(), SIGUSR1);
	}
	/* move TCB to ready queue */
	else {
		pThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pThread);
	}

	return 0;
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

/* wait specific child  thread */
int thread_join(thread_t tid, void** retval) {
	if (!pThreadTbEnt[tid].bUsed) return -1;

	/* child thread status is already zombie */
	if (pThreadTbEnt[tid].pThread->status == THREAD_STATUS_ZOMBIE) {
		*(int*)*retval = pThreadTbEnt[tid].pThread->exitCode;
		if (DeleteThreadFromWaiting(pThreadTbEnt[tid].pThread) == -1) return -1;
		free(pThreadTbEnt[tid].pThread);
		pThreadTbEnt[tid].bUsed = 0;
		pThreadTbEnt[tid].pThread = NULL;
		return 1;
	}
	else {
		pCurrentThead->status = THREAD_STATUS_WAIT;
		InsertThreadIntoWaiting(pCurrentThead);
		pCurrentThead = NULL;
		kill(getppid(), SIGUSR1);
		//signal(SIGCHLD, (void*)exit_signal);
		//kill(getpid(), SIGSTOP);
		/*sigset_t oldset;
		sigemptyset(&oldset);
		sigaddset(&oldset, SIGCHLD);
		sigprocmask(SIG_UNBLOCK, &oldset, NULL);
		signal(SIGCHLD, (void*)exit_signal);
		kill(getpid(), SIGSTOP);*/
		while (1) {
			if (pThreadTbEnt[tid].pThread->exitCode == *(int*)*retval) 
				break;
		}
		thread_t parent_tid = thread_self();
		Thread* pThread = (Thread*)malloc(sizeof(Thread));
		pThread = pThreadTbEnt[parent_tid].pThread;

		/* get thread from waiting queue */
		if (DeleteThreadFromWaiting(pThread) == -1) return -1;

		if (pCurrentThead != NULL && pThread->priority < pCurrentThead->priority) {
			printf("%d : 1\n", getpid());
			InsertThreadToTail(pThread);
			kill(getppid(), SIGUSR1);
		}
		else if (pCurrentThead == NULL) {
			printf("%d : 2\n", getpid());
			InsertThreadToTail(pThread);
			kill(getppid(), SIGUSR1);
		}
		/* move TCB to ready queue */
		else {
			printf("%d : 3\n", getpid());
			pThread->status = THREAD_STATUS_READY;
			InsertThreadToTail(pThread);
			kill(getppid(), SIGSTOP);
		}
		*(int*)*retval = pThreadTbEnt[tid].pThread->exitCode;
		if (DeleteThreadFromWaiting(pThreadTbEnt[tid].pThread) == -1) return -1;
		free(pThreadTbEnt[tid].pThread);
		pThreadTbEnt[tid].bUsed = 0;
		pThreadTbEnt[tid].pThread = NULL;
	}
}

/* thread terminate itself */
int thread_exit(void* retval) {
	printf("%d : exit\n", getpid());
	thread_t tid = thread_self();
	pThreadTbEnt[tid].pThread->exitCode = *(int*)retval;
	pThreadTbEnt[tid].pThread->status = THREAD_STATUS_ZOMBIE;
	InsertThreadIntoWaiting(pThreadTbEnt[tid].pThread);
	pCurrentThead = NULL;
	//kill(getppid(), SIGUSR2);
	exit(0);

	return 0;
}
