#include "Init.h"
#include "Thread.h"
#include "Scheduler.h"
#include <string.h>

//////////////////////////////////
/* 이거 헤더 추가하는건지 확인하기 */
#include "MsgQueue.h"
//////////////////////////////////
void Init(void) {
    for(int i = 0; i < MAX_QCB_NUM; i++){
        strcpy(qcbTblEntry[i].name, "null");
        qcbTblEntry[i].mode = 0;
        qcbTblEntry[i].openCount = 0;
        qcbTblEntry[i].pQcb = NULL;
        qcbTblEntry[i].bUsed = 0;
    }
}
