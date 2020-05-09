#include "TestCase1.h"

void* Tc1ThreadProc(void* param)
{
	thread_t tid = 0;
	int count = 0;
	int i;
	int* retVal;

	tid = thread_self();

	for(int i=0;i<5;i++){
		sleep(2);
		printf("%d : Tc1ThreadProc: my thread id (%d), arg is (%d)\n", getpid(), (int)tid, *((int*)param));
		count++;
	}

	retVal = (int*)param;
	thread_exit(retVal);
	return NULL;
}

void TestCase1(void)
{
	thread_t tid[TOTAL_THREAD_NUM];
	int result[TOTAL_THREAD_NUM];

	int i = 0, i1 = 1, i2 = 2, i3 = 3, i4 = 4, i5 = 5;

	thread_create(&tid[0], NULL, 1, (void*)Tc1ThreadProc,(void*) &i1);	
	thread_create(&tid[1], NULL, 1, (void*)Tc1ThreadProc,(void*) &i2);	
	thread_create(&tid[2], NULL, 1, (void*)Tc1ThreadProc,(void*) &i3);	
	thread_create(&tid[3], NULL, 1, (void*)Tc1ThreadProc,(void*) &i4);	
	thread_create(&tid[4], NULL, 1, (void*)Tc1ThreadProc,(void*) &i5);

	printf("==================TID TABLE==================\n");
	for (thread_t id = 0; id < MAX_THREAD_NUM; id++) {
		if (pThreadTblEnt[id].bUsed == 0) continue;
		printf("[TID %d] : PID = %d(%p), PRIORITY = %d\n", id, pThreadTblEnt[id].pThread->pid, pThreadTblEnt[id].pThread, pThreadTblEnt[id].pThread->priority);
	}


	for(i=0;i<TOTAL_THREAD_NUM;i++)
	{
		int* retVal;
		thread_join(tid[i],(void **)&retVal);

		printf("%d : Thread [ %d ] is finish Return : [ %d ] \n",getpid(), (int)tid[i], *retVal);
	}

	return ;
}

