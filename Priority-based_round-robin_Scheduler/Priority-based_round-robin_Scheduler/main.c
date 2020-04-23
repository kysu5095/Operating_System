#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "test.h"

#include<stdio.h>

int main(void)
{
	thread_t tid;
	int arg = 5;
	Init();
	printf("%d : start program\n", getpid());
	thread_create(&tid, NULL, 7, (void*)TestCase1, (void*)0);
	RunScheduler();
	while (1) {}
}


