#define _GNU_SOURCE
#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"

/*
 InsertThreadToTail      : ready queue�� �켱������ �´� ���̺��� thread�� 'tail'�� ����    
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
				if (pThreadTblEnt[id].bUsed == 0) continue;
				if (pThreadTblEnt[id].pThread->pid == tmp->pid) {
					//free(pThreadTblEnt[id].pThread);
					pThreadTblEnt[id].pThread = tmp;
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
	// when waiting queue empty
	if (pWaitingQueueHead == NULL) {
		pWaitingQueueHead = pThread;
		pWaitingQueueHead->phPrev = NULL;
		pWaitingQueueHead->phNext = NULL;
	}
	else {
		// when waiting queue have only head
		if (pWaitingQueueTail == NULL) {
			pWaitingQueueTail = pThread;
			pWaitingQueueTail->phNext = NULL;
			pWaitingQueueTail->phPrev = pWaitingQueueHead;
			pWaitingQueueHead->phNext = pWaitingQueueTail;
		}
		else {
			Thread* tmp = (Thread*)malloc(sizeof(Thread));
			tmp->stackSize = pWaitingQueueTail->stackSize;
			tmp->stackAddr = pWaitingQueueTail->stackAddr;
			tmp->status = pWaitingQueueTail->status;
			tmp->exitCode = pWaitingQueueTail->exitCode;
			tmp->pid = pWaitingQueueTail->pid;
			tmp->priority = pWaitingQueueTail->priority;
			tmp->phPrev = pWaitingQueueTail->phPrev;
			pWaitingQueueTail->phPrev->phNext = tmp;
			pWaitingQueueTail = pThread;
			pWaitingQueueTail->phPrev = tmp;
			pWaitingQueueTail->phNext = NULL;
			tmp->phNext = pWaitingQueueTail;

			/* update thread table */
			for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
				if (pThreadTblEnt[id].bUsed == 0) continue;
				if (pThreadTblEnt[id].pThread->pid == tmp->pid) {
					//free(pThreadTblEnt[id].pThread);
					pThreadTblEnt[id].pThread = tmp;
					break;
				}
			}
		}
	}
}

/* delete thread form waiting queue */
BOOL DeleteThreadFromWaiting(Thread* pThread) {
	Thread *tmp = (Thread*)malloc(sizeof(Thread));
	tmp = pWaitingQueueHead;
	while (tmp != NULL) {
		if (tmp == pThread) {
			// When delete head
			if (tmp == pWaitingQueueHead) {
				// When waiting queue size is 1
				if (tmp->phNext == NULL) {
					pWaitingQueueHead = NULL;
					pWaitingQueueTail = NULL;
				}
				else {
					// When waiting queue size is 2
					if (tmp->phNext == pWaitingQueueTail) {
						pWaitingQueueHead = pWaitingQueueTail;
						pWaitingQueueHead->phNext = NULL;
						pWaitingQueueHead->phPrev = NULL;
						pWaitingQueueTail = NULL;
					}
					else {
						pWaitingQueueHead = pWaitingQueueHead->phNext;
						pWaitingQueueHead->phPrev = NULL;
					}
				}
			}
			else {
				// When delete tail
				if (tmp == pWaitingQueueTail) {
					// When waiting queue size is 2
					if (tmp->phPrev == pWaitingQueueHead) {
						pWaitingQueueHead->phNext = NULL;
						pWaitingQueueTail = NULL;
					}
					else {
						pWaitingQueueTail = pWaitingQueueTail->phPrev;
						pWaitingQueueTail->phNext = NULL;
					}
				}
				else {
					tmp->phPrev->phNext = tmp->phNext;
					tmp->phNext->phPrev = tmp->phPrev;
				}
			}
			return 1;
		}
		if (tmp->phNext == NULL) break;
		tmp = tmp->phNext;
	}
	return 0;
}

/* get thread id from thread table */
thread_t set_threadID(Thread* pThread) {
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (pThreadTblEnt[id].bUsed == 0) {
			pThreadTblEnt[id].bUsed = 1;
			pThreadTblEnt[id].pThread = pThread;
			return id;
		}
	}
	return 1;
}

/* create thread */
int thread_create(thread_t *thread, thread_attr_t *attr, int priority, void *(*start_routine) (void *), void *arg){
	void* pStack;
	pStack = malloc(STACK_SIZE);
	int flags = SIGCHLD|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_VM;
	pid_t pid = clone((void*)start_routine, (char*)pStack+STACK_SIZE, flags, arg);
    /* stop thread immediately */
	kill(pid, SIGSTOP);
	/* allocate TCB && init TCB */
	Thread* pThread    = (Thread*)malloc(sizeof(Thread));
	pThread->stackSize = STACK_SIZE;
	pThread->stackAddr = (char*)pStack + STACK_SIZE;
	pThread->exitCode  = -1;
	pThread->pid       = pid;
	pThread->priority  = priority;
	pThread->phNext    = NULL;
	pThread->phPrev    = NULL;
	/*thread_t tid	   = set_threadID(pThread);
	*thread			   = tid;*/
	*thread = 1;
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (pThreadTblEnt[id].bUsed == 0) {
			pThreadTblEnt[id].bUsed = 1;
			pThreadTblEnt[id].pThread = pThread;
			*thread = id;
			break;
		}
	}
	/* context switching */
	if (pCurrentThread != NULL && priority < pCurrentThread->priority) {
        pCurrentThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pCurrentThread);
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
	if (pThreadTblEnt[tid].bUsed == 0) return -1;
	/* thread status is not ready */
	if (pThreadTblEnt[tid].pThread->status != THREAD_STATUS_READY) return -1;
	if (DeleteThreadFromReady(pThreadTblEnt[tid].pThread)) {
		pThreadTblEnt[tid].pThread->status = THREAD_STATUS_WAIT;
		InsertThreadIntoWaiting(pThreadTblEnt[tid].pThread);
		return 0;
	}
	else
		return -1;
}
 
/* kill thread */
/* note : don't kill oneself!!!! */
int thread_cancel(thread_t tid){
	if (!pThreadTblEnt[tid].bUsed) return -1;
	pid_t pid = pThreadTblEnt[tid].pThread->pid;
	/* kill thread */
	kill(pid, SIGKILL);
	if (pThreadTblEnt[tid].pThread->status == THREAD_STATUS_READY)
		DeleteThreadFromReady(pThreadTblEnt[tid].pThread);
	if (pThreadTblEnt[tid].pThread->status == THREAD_STATUS_WAIT)
		DeleteThreadFromWaiting(pThreadTblEnt[tid].pThread);
	pThreadTblEnt[tid].pThread->status = THREAD_STATUS_ZOMBIE;
	free(pThreadTblEnt[tid].pThread);
	pThreadTblEnt[tid].bUsed = 0;
	pThreadTblEnt[tid].pThread = NULL;

	return 0;
}

/* resume thread */
int thread_resume(thread_t tid){
	if (pThreadTblEnt[tid].bUsed == 0) return -1;
	Thread* pThread = (Thread*)malloc(sizeof(Thread));
	pThread = pThreadTblEnt[tid].pThread;
	/* get thread from waiting queue */
	if (DeleteThreadFromWaiting(pThread) == 0) return -1;
	/* context switching */
	if (pCurrentThread != NULL && pThread->priority < pCurrentThread->priority) {
		alarm(0);
		int suspend_pid = pCurrentThread->pid;
		pCurrentThread->status = THREAD_STATUS_READY;
		InsertThreadToTail(pCurrentThread);
		pCurrentThread = pThread;
		pCurrentThread->status = THREAD_STATUS_RUN;
		kill(pCurrentThread->pid, SIGCONT);
		alarm(TIMESLICE);
		kill(suspend_pid, SIGSTOP);
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
	int pid = getpid();
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (pThreadTblEnt[id].bUsed == 0) continue;
		if (pThreadTblEnt[id].pThread->pid == pid)
			return id;
	}

	return -1;
}

/* signal handler for thread join */
void sigchld_handler(int signo) {
	// just parent known child thread exit
}

/* wait	specific child  thread */
int thread_join(thread_t tid, void** retval) {
	 if (!pThreadTblEnt[tid].bUsed) return -1;

	/* when child thread is zombie */
	if (pThreadTblEnt[tid].pThread->status != THREAD_STATUS_ZOMBIE) {
		if (pCurrentThread != NULL) {
			pCurrentThread->status = THREAD_STATUS_WAIT;
			InsertThreadIntoWaiting(pCurrentThread);
			pCurrentThread = NULL;
		}
		/* only receive exit signal of SIGCHLD */
		sigaction(SIGCHLD, &(struct sigaction){.sa_handler = sigchld_handler, .sa_flags = SA_NOCLDSTOP}, NULL);
		kill(getppid(), SIGUSR1);
		pause();
	}
	/* store exitCode */
	*retval = &pThreadTblEnt[tid].pThread->exitCode;
	if (DeleteThreadFromWaiting(pThreadTblEnt[tid].pThread) == 0) return -1;
	free(pThreadTblEnt[tid].pThread);
	pThreadTblEnt[tid].bUsed = 0;
	pThreadTblEnt[tid].pThread = NULL;
}

/* thread terminate itself */
int thread_exit(void* retval) {
	alarm(0);
	thread_t tid = thread_self();
	pThreadTblEnt[tid].pThread->exitCode = (int*)retval;
	pThreadTblEnt[tid].pThread->status = THREAD_STATUS_ZOMBIE;
	InsertThreadIntoWaiting(pThreadTblEnt[tid].pThread);
	/* context switching */
	__ContextSwitch(getpid(), getppid());
	/* call exit */
	exit(0);
	/* never execute below code */
	return 0;
}
