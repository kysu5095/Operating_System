#include "MsgQueue.h"

pmqd_t pmq_open(const char* name, int flags, mode_t perm, pmq_attr* attr) {
    /* check message queue */
    for(int i = 0; i < MAX_QCB_NUM; i++){
        if(strcmp(qcbTblEntry[i].name, name) == 0)
            return i;
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
        perror("no empty row");
        return -1;
    }
    /* create Qcb */  
    Qcb* qcb = (Qcb*)malloc(sizeof(Qcb));
    qcb->msgCount = 0;
    qcb->pMsgHead = NULL;
    qcb->pMsgTail = NULL;
    qcb->waitThreadCount = 0;
    qcb->pWaitQHead = NULL;
    qcb->pWaitQTail = NULL;
    /* insert Qcb to QcbTblEntry table */
    strcpy(qcbTblEntry[num].name, name);
    qcbTblEntry[num].mode = (int)perm;
    qcbTblEntry[num].openCount++;
    qcbTblEntry[num].pQcb = qcb;
    qcbTblEntry[num].bUsed = 1;
}

int pmq_close(pmqd_t mqd) {

}
int pmq_send(pmqd_t mqd, char* msg_ptr, size_t msg_len, unsigned int msg_prio) {

}
ssize_t pmq_receive(pmqd_t mqd, char* msg_ptr, size_t msg_len, unsigned int* msg_prio)
    
}
