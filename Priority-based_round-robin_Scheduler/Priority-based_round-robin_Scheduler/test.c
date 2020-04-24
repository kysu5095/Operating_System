#include "test.h"

int foo1(void* param) {
	thread_t tid = 0;
	int cnt = 0;
	int exitCode = 1;
	tid = thread_self();
	cnt = 5;
	while (cnt) {
		printf("%d : foo1 exe my tid is (%d),  cnt is (%d)\n", getpid(), tid, cnt);
		cnt--;
		sleep(1);
	}
	//thread_exit(&exitCode);
}

int foo2(void* param) {
	thread_t tid = 0;
	int cnt = 0;
	tid = thread_self();
	cnt = 5;
	while (cnt) {
		printf("%d : foo2 exe my tid is (%d),  cnt is (%d)\n", getpid(), tid, cnt);
		cnt--;
		sleep(1);
	}
}

int foo3(void* param) {
	thread_t tid = 0;
	int cnt = 0;
	tid = thread_self();
	cnt = 5;
	while (cnt) {
		printf("%d : foo3 exe my tid is (%d),  cnt is (%d)\n", getpid(), tid, cnt);
		cnt--;
		sleep(1);
	}
}

int foo4(void* param) {
	thread_t tid = 0;
	int cnt = 0;
	tid = thread_self();
		cnt = 5;
	while (cnt) {
		printf("%d : foo4 exe my tid is (%d),  cnt is (%d)\n", getpid(), tid, cnt);
		cnt--;
		sleep(1);
	}
}

void TestCase1(void) {
	printf("%d : run testcase\n", getpid());
	fflush(stdout);
	int a = 10, b = 20, c = 30, d = 40;
	int exitCode = 1;
	thread_t t1, t2, t3, t4;
	printf("%d : start\n", getpid());
	fflush(stdout);
	thread_create(&t1, NULL, 3, (void*)foo1, &a);
	//printf("%d : t1\n", getpid());
	fflush(stdout);
	thread_create(&t2, NULL, 3, (void*)foo2, &a);
	//printf("%d : t2\n", getpid());
	fflush(stdout);
	thread_create(&t3, NULL, 3, (void*)foo3, &a);
	//printf("%d : t3\n", getpid());
	fflush(stdout);
	//printf("%d : thread %d create", getpid(), t1);
	//fflush(stdout);
	//thread_join(t1, (void**)exitCode);
	//sleep(3);
	//printf("0");
	//kill(getpid(), SIGSTOP);
	//printf("1");
	////thread_suspend((thread_t)1);
	//thread_create(&t2, NULL, 2, (void*)foo2, &b);
	//printf("2");
	//sleep(2);
	//thread_resume((thread_t)1);
	//sleep(3);
	//thread_cancel((thread_t)1);
	//thread_create(&t3, NULL, 3, (void*)foo3, &c);
	//sleep(2);
	//thread_suspend((thread_t)2);
	//sleep(1);
	//thread_cancel((thread_t)3);
	//thread_create(&t4, NULL, 4, (void*)foo4, &d);
	printf("END\n");
	while (1) {}
}
