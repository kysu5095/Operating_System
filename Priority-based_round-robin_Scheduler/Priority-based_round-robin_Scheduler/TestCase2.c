#include "TestCase2.h"


void* Tc2ThreadProc(void* param)
{
	thread_t tid = 0;
	int *retVal;
	tid = thread_self();
	while (1)
	{
		printf("Tc2ThreadProc: my thread id:(%d), arg : %d | pid : %d\n", (int)tid, *((int*)param), getpid());
		sleep(2);
	}
	return retVal;
}

void TestCase2(void)
{

	thread_t tid[TOTAL_THREAD_NUM];
	int i = 0;
	int j = 0;
	int i1 = 1, i2 = 2, i3 = 3, i4 = 4;


	thread_create(&tid[0], NULL, 0, (void*)Tc2ThreadProc, (void*)&i1);
	thread_create(&tid[1], NULL, 0, (void*)Tc2ThreadProc, (void*)&i2);
	thread_create(&tid[2], NULL, 0, (void*)Tc2ThreadProc, (void*)&i3);
	thread_create(&tid[3], NULL, 0, (void*)Tc2ThreadProc, (void*)&i4);

	/* Suspend all thread */
	for (i = 0; i < TOTAL_THREAD_NUM; i++)
	{
		sleep(2);

		if (thread_suspend(tid[i]) == -1)
		{
			printf("TestCase2: Thread suspending Failed..\n");
			assert(0);
		}

		Thread *temp = pWaitingQueueHead->phNext;

		printf("current waiting queue : ");
		for (; temp != pWaitingQueueTail; temp = temp->phNext) {
			printf(" %d", temp->pid);
			if (temp->status != 2)
			{
				printf("TestCase2: Thread is not suspended\n");
				assert(0);
			}

		}
		printf("\n");
	}

	/* Resume thread sequentially */
	for (i = 0; i < TOTAL_THREAD_NUM; i++)
	{
		sleep(2);

		if (thread_resume(tid[i]) == -1)
		{
			printf("Testcase2: Thread resume Failed\n");
			assert(0);
		}
		Thread *temp = pWaitingQueueHead->phNext;

		printf("current waiting queue : ");
		for (; temp != pWaitingQueueTail; temp = temp->phNext) {
			printf(" %d", temp->pid);
		}
		printf("\n");
	}

	for (i = TOTAL_THREAD_NUM - 1; i >= 0; i--)
	{
		sleep(2);

		if (thread_suspend(tid[i]) == -1)
		{
			printf("TestCase2: Thread suspending Failed..\n");
			assert(0);
		}

		Thread *temp = pWaitingQueueHead->phNext;

		printf("current waiting queue : ");
		for (; temp != pWaitingQueueTail; temp = temp->phNext) {
			printf(" %d", temp->pid);
			if (temp->status != 2)
			{
				printf("TestCase2: Thread is not suspended\n");
				assert(0);
			}
		}
		printf("\n");
	}

	/* Resume thread sequentially */
	for (i = TOTAL_THREAD_NUM - 1; i >= 0; i--)
	{
		sleep(2);

		if (thread_resume(tid[i]) == -1)
		{
			printf("Testcase2: Thread resume Failed\n");
			assert(0);
		}

		Thread *temp = pWaitingQueueHead->phNext;

		printf("current waiting queue : ");
		for (; temp != pWaitingQueueTail; temp = temp->phNext) {
			printf(" %d", temp->pid);
		}
		printf("\n");
	}
	return;
}
