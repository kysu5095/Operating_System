#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <stdlib.h>

void Init(void) {
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
}