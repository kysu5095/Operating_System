#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "test.h"

#include<stdio.h>

void interruptHandler(int sig) {
	printf("%d : Handler run %d\n", getpid(), sig);

}

int main(void)
{
	thread_t tid;
	int arg = 5;
	Init();
	signal(SA_NOCLDSTOP, interruptHandler);
	printf("%d : start program\n", getpid());
	thread_create(&tid, NULL, 1, (void*)TestCase1, (void*)0);
	RunScheduler();
	while (1) {}
}


