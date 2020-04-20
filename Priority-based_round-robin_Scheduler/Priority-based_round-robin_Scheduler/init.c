#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"

void Init(void) {
	/* register signal handler */
	signal(SIGALRM, (void*)RunScheduler);

	/* running thread init */
	pCurrentThead = NULL;

	/* thread table init */
	for (int i = 0; i < MAX_THREAD_NUM; i++) {
		pThreadTbEnt[i].bUsed = 0;
		pThreadTbEnt[i].pThread = NULL;
	}

	/* ready queue init */
	for (int i = 0; i < MAX_READYQUEUE_NUM; i++) {
		pReadyQueueEnt[i].queueCount = 0;
		pReadyQueueEnt[i].pHead = NULL;
		pReadyQueueEnt[i].pTail = NULL;
	}

	/* waiting queue init */
	pWaitingQueueHead = (Thread*)malloc(sizeof(Thread));
	pWaitingQueueTail = (Thread*)malloc(sizeof(Thread));
	pWaitingQueueHead->phNext = pWaitingQueueTail;
	pWaitingQueueHead->phPrev = NULL;
	pWaitingQueueTail->phNext = NULL;
	pWaitingQueueTail->phPrev = pWaitingQueueHead;

	/* main thread init */
	/*Thread* mainThread = (Thread*)malloc(sizeof(Thread));
	mainThread->stackSize = NULL;
	mainThread->stackAddr = NULL;
	mainThread->status    = THREAD_STATUS_RUN;
	mainThread->exitCode  = NULL;
	mainThread->pid       = getpid();
	mainThread->priority  = 8;
	mainThread->phNext    = NULL;
	mainThread->phPrev    = NULL;
	pCurrentThead         = mainThread;
	pThreadTbEnt[0].bUsed = 1;
	pThreadTbEnt[0].pThread = mainThread;*/
}