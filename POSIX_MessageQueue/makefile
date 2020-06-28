exefile : init.o msgqueue.o scheduler.o thread.o main.o
	gcc -o exefile init.o msgqueue.o scheduler.o thread.o main.o
init.o : Init.c
	gcc -c -o init.o Init.c
msgqueue.o : MsgQueue.c
	gcc -c -o msgqueue.o MsgQueue.c
scheduler.o : Scheduler.c
	gcc -c -o scheduler.o Scheduler.c
thread.o : Thread.c
	gcc -c -o thread.o Thread.c
main.o : main.c
	gcc -c -o main.o main.c
clean :
	rm *.o exefile
