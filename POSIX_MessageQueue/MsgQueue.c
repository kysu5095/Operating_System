#include "MsgQueue.h"
#include <string.h>
#include <stdio.h>

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
        while(cur != NULL && cur->priority >= msg->priority)
            cur = cur->pNext;
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

void insertThread(pmqd_t mqd){

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
    return 0;
}

int pmq_send(pmqd_t mqd, char* msg_ptr, size_t msg_len, unsigned int msg_prio) {
    /* create message */
    Message* msg = (Message*)malloc(sizeof(Message));
    strcpy(msg->data, msg_ptr);
    msg->size = (int)msg_len;
    msg->priority = msg_prio;
    msg->pNext = NULL;
    msg->pPrev = NULL;
    insertQcb(mqd, msg);
    /////////////////////////
    /* Waiting Thread 찾기 */
    /////////////////////////
    return 0;
}

ssize_t pmq_receive(pmqd_t mqd, char* msg_ptr, size_t msg_len, unsigned int* msg_prio) {
    if(qcbTblEntry[mqd].bUsed == 0){
        perror("pmq_receive : qcbTblEntry is not used");
        return -1;
    }
    Message* msg = deleteQcb(mqd);
    if(msg == NULL){

    }
    else{
        strcpy(msg_ptr, msg->data);
        msg_prio = msg->priority;
        free(msg);
        return msg->size;
    }
}
