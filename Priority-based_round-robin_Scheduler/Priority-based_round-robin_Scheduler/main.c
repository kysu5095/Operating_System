#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "test.h"

#include<stdio.h>

//int AppTask(void* param) {
//	thread_t tid1, tid2, tid3;
//	int flag = 1;
//	printf("%d : AppTask\n", getpid());
//	thread_create(&tid1, NULL, 2, (void*)foo1, &flag);
//	printf("%d : finish 1_thread\n", getpid());
//	flag++;
//	thread_create(&tid2, NULL, 2, (void*)foo1, &flag);
//	printf("%d : finish 2_thread\n", getpid());
//	flag++;
//	thread_create(&tid3, NULL, 2, (void*)foo1, &flag);
//	printf("%d : finish 3_thread\n", getpid());
//	while (1) {}
//}

int main(void)
{
	thread_t tid;
	int arg = 5;
	Init();
	printf("%d : start program\n", getpid());
	thread_create(&tid, NULL, 2, (void*)TestCase1, (void*)0);
	RunScheduler();
	while (1) {}
}


