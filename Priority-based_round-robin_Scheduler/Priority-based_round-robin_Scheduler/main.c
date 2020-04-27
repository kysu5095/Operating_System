#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"
#include "test.h"

int main(void)
{
	thread_t tid;
	int arg = 5;
	Init();
	printf("%d : start program\n", getpid());
	thread_create(&tid, NULL, 3, (void*)TestCase1, (void*)0);
	RunScheduler();
	while (1) {}
}


