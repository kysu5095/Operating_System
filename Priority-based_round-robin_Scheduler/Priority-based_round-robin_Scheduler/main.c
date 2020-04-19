#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"

#include<stdio.h>

int foo1(int param) {
	thread_t tid = 0;
	int count = 0;
	tid = thread_self();
	count = 5;
	while (count > 0) {
		sleep(1);
		printf("my thread id : %d,  parameter : %d\n", tid, param);
		count--;
	}
}

int AppTask(void* param) {
	thread_t tid1, tid2, tid3;
	int flag = 1;
	printf("%d : AppTask\n", getpid());
	thread_create(&tid1, NULL, 1, (void*)foo1, (void*)flag);
	printf("%d : create 1thread\n", getpid());
	flag++;
	thread_create(&tid2, NULL, 1, (void*)foo1, (void*)flag);
	printf("%d : create 2thread\n", getpid());
	flag++;
	thread_create(&tid3, NULL, 1, (void*)foo1, (void*)flag);
	printf("%d : create 3thread\n", getpid());
	while (1) {}
}

int main(void)
{
	thread_t tid;
	int arg = 5;
	Init();
	printf("%d start program\n", getpid());
	thread_create(&tid, NULL, 4, (void*)AppTask, (void*)arg);
	printf("%d : before first call scheduler\n", getpid());
	RunScheduler();
	while (1) {}
}


