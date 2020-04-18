#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"

#include<stdio.h>

void* foo1(void* arg) {
	printf("foo1\n");
	while (1) {}
}

void* foo2(void* arg) {
	printf("foo2\n");
	while (1) {}
}

void* foo3(void* arg) {
	printf("foo3\n");
	while (1) {}
}

void* AppTask(void* param) {
	thread_t tid1, tid2, tid3;
	int arg = 100;
	printf("AppTask\n");
	thread_create(&tid1, NULL, 1, foo1, &arg);
	thread_create(&tid2, NULL, 1, foo2, &arg);
	thread_create(&tid3, NULL, 1, foo3, &arg);
	while (1);
}

int main(void)
{
	signal(SIGALRM, RunScheduler);
	thread_t tid;
	int arg = 100;
	Init();
	thread_create(&tid, NULL, 4, AppTask, &arg);
	alarm(TIMESLICE);
	while (1);
}


