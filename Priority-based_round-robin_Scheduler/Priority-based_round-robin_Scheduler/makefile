exefile : test.o init.o scheduler.o thread.o main.o
	gcc -o exefile test.o init.o  scheduler.o thread.o main.o
test.o : test.c
	gcc -c -o test.o test.c
init.o : init.c
	gcc -c -o init.o init.c
scheduler.o : Scheduler.c
	gcc -c -o scheduler.o Scheduler.c
thread.o : Thread.c
	gcc -c -o thread.o Thread.c
main.o : main.c
	gcc -c -o main.o main.c
clean :
	rm *.o exefile
