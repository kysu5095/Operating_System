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
		printf("%d : my thread id : %d   count : %d\n", getpid(), tid, count);
		sleep(1);
		count--;
	}
	while (1) {}
}

int AppTask(void* param) {
	thread_t tid1, tid2, tid3;
	int flag = 1;
	printf("%d : AppTask\n", getpid());
	thread_create(&tid1, NULL, 2, (void*)foo1, &flag);
	printf("%d : finish 1_thread\n", getpid());
	flag++;
	thread_create(&tid2, NULL, 2, (void*)foo1, &flag);
	printf("%d : finish 2_thread\n", getpid());
	flag++;
	thread_create(&tid3, NULL, 2, (void*)foo1, &flag);
	printf("%d : finish 3_thread\n", getpid());
	while (1) {}
}

int main(void)
{
	if (signal(SIGALRM, (void*)RunScheduler) == SIG_ERR) {
		perror("SIGALRM ERROR");
	}
	thread_t tid;
	int arg = 5;
	Init();
	printf("%d : start program\n", getpid());
	thread_create(&tid, NULL, 3, (void*)AppTask, &arg);
	RunScheduler();
	while (1) {}
}


