#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"

void Init(void) {
	/* register signal handler */
	signal(SIGALRM, (void*)RunScheduler);
	signal(SIGUSR1, (void*)RunScheduler);
	//signal(SIGCHLD, SIG_IGN);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGCHLD);
	sigprocmask(SIG_BLOCK, &set, NULL);

	/* running thread init */
	pCurrentThread = NULL;

	/* thread table init */
	for (int i = 0; i < MAX_THREAD_NUM; i++) {
		pThreadTblEnt[i].bUsed = 0;
		pThreadTblEnt[i].pThread = NULL;
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
}