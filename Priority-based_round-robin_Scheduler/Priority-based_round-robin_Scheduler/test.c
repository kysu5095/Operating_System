#include "test.h"

int foo1(void* param) {
	thread_t tid = 0;
	int cnt = 0;
	tid = thread_self();
	cnt = 5;
	while (cnt) {
		printf("%d : foo1 exe my tid is (%d),  cnt is (%d)\n", getpid(), tid, cnt);
		cnt--;
		sleep(1);
	}
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
void TestCase1(void) {
	int a = 10, b = 20, c = 30;
	thread_t t1, t2, t3;
	thread_create(&t1, NULL, 3, (void*)foo1, &a);
	thread_create(&t2, NULL, 3, (void*)foo2, &b);
	thread_create(&t3, NULL, 3, (void*)foo3, &c);
	while (1) {}
}
