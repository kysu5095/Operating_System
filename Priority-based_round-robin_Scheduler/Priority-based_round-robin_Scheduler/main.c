
#include<stdio.h>
#include<stdlib.h>

#include "test.h"

#include "Init.h"
#include "Scheduler.h"
#include "Thread.h"

int main(int argc, char* argv[]) {

	int TcNum;
	thread_t tid1, tid2, tid3, tid4, tid5;

	if (argc != 2) {
		perror("Input TestCase Number!");
		exit(0);
	}

	Init();

	TcNum = atoi(argv[1]);


	switch (TcNum)
	{
	case 1:
		thread_create(&tid1, NULL, 0, (void*)TestCase1, 0);
		break;
	}

	RunScheduler();
	while (1) {}

	return 0;
}