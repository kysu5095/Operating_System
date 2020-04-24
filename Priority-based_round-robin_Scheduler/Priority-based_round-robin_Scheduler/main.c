#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "test.h"

#include<stdio.h>

void interruptHandler() {
	printf("%d : interrupt\n", getpid());
	fflush(stdout);
}

int main(void)
{
	signal(SA_NOCLDWAIT, interruptHandler);
	thread_t tid;
	int arg = 5;
	Init();
	printf("%d : start program\n", getpid());
	thread_create(&tid, NULL, 3, (void*)TestCase1, (void*)0);
	RunScheduler();
	while (1) {}
}


