#include "test.h"

void threadF(int signo) {
	int cnt = 0;
	p_thread tid = 0;
	tid = thread_self();
	cnt = 5;
	while (cnt > 0) {
		sleep(1);
		printf("my thread id (%d) arg is(%d)\n", tid, signo);
		cnt--;
	}
	while (1) {}
}

void main(void) {
	signal(SIGALRM, threadF);
	/*int pid;
	int arg = 100;
	int flags = SIGCHLD|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_VM;
	char* pStack;
	pStack = malloc(STACK_SIZE);
	pid = clone(&threadF, (char*)pStack+STACK_SIZE, flags, &arg);*/
	//kill(pid, SIGSTOP);
	alarm(2);
}
