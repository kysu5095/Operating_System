exe : init.o msgqueue.o scheduler.o thread.o test1.o test2.o test3.o test4.o main.o
	gcc -o exe init.o msgqueue.o scheduler.o thread.o test1.o test2.o test3.o test4.o main.o
init.o : Init.c
	gcc -c -o init.o Init.c
msgqueue.o : MsgQueue.c
	gcc -c -o msgqueue.o MsgQueue.c
scheduler.o : Scheduler.c
	gcc -c -o scheduler.o Scheduler.c
thread.o : Thread.c
	gcc -c -o thread.o Thread.c
test1.o : TestCase1.c
	gcc -c -o test1.o TestCase1.c
test2.o : TestCase2.c
	gcc -c -o test2.o TestCase2.c
test3.o : TestCase3.c
	gcc -c -o test3.o TestCase3.c
test4.o : TestCase4.c
	gcc -c -o test4.o TestCase4.c			
main.o : main.c
	gcc -c -o main.o main.c
clean :
	rm *.o exe
