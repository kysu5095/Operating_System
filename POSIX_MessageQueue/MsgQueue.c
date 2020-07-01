#include "MsgQueue.h"
#include <string.h>
#include <stdio.h>

void insertThreadToReady(Thread* pThread){
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

void deleteThreadToReady(Thread* pThread){
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
			return;
		}
		if (tmp->phNext == NULL) break;
		tmp = tmp->phNext;
	}
	return;
}

Thread* GetThreadToReady() {
	Thread* pThread = (Thread*)malloc(sizeof(Thread));
	for (thread_t idx = 0; idx < MAX_READYQUEUE_NUM; idx++) {
		if (pReadyQueueEnt[idx].queueCount == 0) continue;
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

void insertQcb(pmqd_t mqd, Message* msg){
    Qcb* qcb = qcbTblEntry[mqd].pQcb;
    qcb->msgCount++;
    /* msg list is empty */
    if(qcb->pMsgHead->pNext == qcb->pMsgTail){
        qcb->pMsgHead->pNext = msg;
        qcb->pMsgTail->pPrev = msg;
        msg->pPrev = qcb->pMsgHead;
        msg->pNext = qcb->pMsgTail;
    }
    else{
        Message* cur = qcb->pMsgHead->pNext;
        while(cur != qcb->pMsgTail && cur->priority >= msg->priority){
            cur = cur->pNext;
        }
        msg->pPrev = cur->pPrev;
        msg->pNext = cur;
        cur->pPrev->pNext = msg;
        cur->pPrev = msg;
    }
}

Message* deleteQcb(pmqd_t mqd){
    Qcb* qcb = qcbTblEntry[mqd].pQcb;
    /* msg list is empty */
    if(qcb->msgCount == 0) return NULL;
    qcb->msgCount--;
    Message* msg = qcb->pMsgHead->pNext;
    qcb->pMsgHead->pNext = msg->pNext;
    msg->pNext->pPrev = qcb->pMsgHead;
    msg->pNext = NULL;
    msg->pPrev = NULL;
    return msg;
}

void insertThread(pmqd_t mqd, Thread* pThread){
    Qcb* qcb = qcbTblEntry[mqd].pQcb;
    qcb->waitThreadCount++;
    /* wait list is empty */
    if(qcb->pWaitQHead->phNext == qcb->pWaitQTail){
        qcb->pWaitQHead->phNext = pThread;
        qcb->pWaitQTail->phPrev = pThread;
        pThread->phPrev = qcb->pWaitQHead;
        pThread->phNext = qcb->pWaitQTail;
    }
    else{
        qcb->pWaitQTail->phPrev->phNext = pThread;
        pThread->phPrev = qcb->pWaitQTail->phPrev;
        pThread->phNext = qcb->pWaitQTail;
        qcb->pWaitQTail->phPrev = pThread;
    }
}

void deleteThread(pmqd_t mqd){
    Qcb* qcb = qcbTblEntry[mqd].pQcb;
    if(qcb->waitThreadCount == 0) return;
    qcb->waitThreadCount--;
    Thread* pThread = qcb->pWaitQHead->phNext;
    qcb->pWaitQHead->phNext = pThread->phNext;
    pThread->phNext->phPrev = qcb->pWaitQHead;
    insertThreadToReady(pThread);
}

pmqd_t pmq_open(const char* name, int flags, mode_t perm, pmq_attr* attr) {
    /* check message queue */
    for(int i = 0; i < MAX_QCB_NUM; i++){
        if(strcmp(qcbTblEntry[i].name, name) == 0){
            qcbTblEntry[i].openCount++;
            return i;
        }
    }
    /* find empty table row */
    int num = -1;
    for(int i = 0; i < MAX_QCB_NUM; i++){
        if(qcbTblEntry[i].bUsed == 0){
            num = i;
            break;
        }
    }
    if(num == -1){
        perror("pmq_open : no empty row");
        return -1;
    }
    /* create Qcb */  
    Qcb* qcb = (Qcb*)malloc(sizeof(Qcb));
    qcb->msgCount = 0;
    qcb->pMsgHead = (Message*)malloc(sizeof(Message));
    qcb->pMsgTail = (Message*)malloc(sizeof(Message));
    qcb->pMsgHead->pPrev = NULL;
    qcb->pMsgHead->pNext = qcb->pMsgTail;
    qcb->pMsgTail->pPrev = qcb->pMsgHead;
    qcb->pMsgTail->pNext = NULL;
    qcb->waitThreadCount = 0;
    qcb->pWaitQHead = (Thread*)malloc(sizeof(Thread));
    qcb->pWaitQTail = (Thread*)malloc(sizeof(Thread));
    qcb->pWaitQHead->phPrev = NULL;
    qcb->pWaitQHead->phNext = qcb->pWaitQTail;
    qcb->pWaitQTail->phPrev = qcb->pWaitQHead;
    qcb->pWaitQTail->phNext = NULL;
    /* insert Qcb to QcbTblEntry table */
    strcpy(qcbTblEntry[num].name, name);
    qcbTblEntry[num].mode = (int)perm;
    qcbTblEntry[num].openCount++;
    qcbTblEntry[num].pQcb = qcb;
    qcbTblEntry[num].bUsed = 1;
    return num;
}

int pmq_close(pmqd_t mqd) {
    if(qcbTblEntry[mqd].bUsed == 0){
        perror("pmq_close : qcbTblEntry is not used");
        return -1;
    }
    qcbTblEntry[mqd].openCount--;
    if(qcbTblEntry[mqd].openCount == 0){
        if(qcbTblEntry[mqd].pQcb->pMsgHead->pNext == qcbTblEntry[mqd].pQcb->pMsgTail){
            strcpy(qcbTblEntry[mqd].name, "null");
            qcbTblEntry[mqd].mode = 0;
            qcbTblEntry[mqd].openCount = 0;
            /////////////////////////////////////////////////
            /* Qcb안에 있는 내용물들도 free하는 구문 추가하기 */
            /////////////////////////////////////////////////
            free(qcbTblEntry[mqd].pQcb);
            qcbTblEntry[mqd].pQcb = NULL;
            qcbTblEntry[mqd].bUsed = 0;
        }
    }
    return 0;
}

int pmq_send(pmqd_t mqd, char* msg_ptr, size_t msg_len, unsigned int msg_prio) {
    if(qcbTblEntry[mqd].bUsed == 0){
        perror("pmq_send : qcbTblEntry is not used");
        return -1;
    }
    /* create message */
    Message* msg = (Message*)malloc(sizeof(Message));
    strcpy(msg->data, msg_ptr);
    msg->size = (int)msg_len;
    msg->priority = msg_prio;
    msg->pNext = NULL;
    msg->pPrev = NULL;
    insertQcb(mqd, msg);
    deleteThread(mqd);
    return 0;
}

ssize_t pmq_receive(pmqd_t mqd, char* msg_ptr, size_t msg_len, unsigned int* msg_prio) {
    if(qcbTblEntry[mqd].bUsed == 0){
        perror("pmq_receive : qcbTblEntry is not used");
        return -1;
    }
    while(1){
        Message* msg = deleteQcb(mqd);
        if(msg == NULL){
            int tid;
            if((tid = thread_self()) == -1){
                perror("pmq_receive : thread_self error");
                return -1;
            }
            Thread* pThread = pThreadTblEnt[tid].pThread;
            deleteThreadToReady(pThread);
            insertThread(mqd, pThread);

            Thread* nThread = GetThreadToReady();
            __ContextSwitch(pThread->pid, nThread->pid);
            kill(getpid(), SIGSTOP);
        }
        else{
            strcpy(msg_ptr, msg->data);
            *msg_prio = msg->priority;
            int sz = msg->size;
            free(msg);
            return sz;
        }
    }
}
