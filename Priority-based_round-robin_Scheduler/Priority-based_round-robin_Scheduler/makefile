exefile : testcase1.o init.o scheduler.o thread.o testcase1.o testcase2.o testcase3.o testcase4.o main.o
	gcc -o exefile init.o  scheduler.o testcase1.o testcase2.o testcase3.o testcase4.o thread.o main.o
init.o : Init.c
	gcc -c -o init.o Init.c
scheduler.o : Scheduler.c
	gcc -c -o scheduler.o Scheduler.c
thread.o : Thread.c
	gcc -c -o thread.o Thread.c
testcase1.o : TestCase1.c
	gcc -c -o testcase1.o TestCase1.c
testcase2.o : TestCase2.c
	gcc -c -o testcase2.o TestCase2.c
testcase3.o : TestCase3.c
	gcc -c -o testcase3.o TestCase3.c
testcase4.o : TestCase4.c
	gcc -c -o testcase4.o TestCase4.c
main.o : main.c
	gcc -c -o main.o main.c
clean :
	rm *.o exefile
