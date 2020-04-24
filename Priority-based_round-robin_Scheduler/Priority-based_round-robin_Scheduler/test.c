#include "test.h"

int foo1(void* param) {
	thread_t tid = 0;
	int cnt = 0;
	int exitCode = 1;
	tid = thread_self();
	cnt = 4;
	while (cnt) {
		printf("%d : foo1 exe my tid is (%d),  cnt is (%d)\n", getpid(), tid, cnt);
		cnt--;
		sleep(1);
	}
	thread_exit(&exitCode);
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
	int a = 10, b = 20, c = 30, d = 40;
	int exitCode = 1;
	void *tmp = &exitCode;
	thread_t t1, t2, t3, t4;
	thread_create(&t1, NULL, 4, (void*)foo1, &a);
	thread_join(t1, &tmp);
	thread_create(&t2, NULL, 3, (void*)foo2, &a);
	thread_create(&t3, NULL, 3, (void*)foo3, &a);
	while (1) {}
}
