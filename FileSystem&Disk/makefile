exefile : initialize.o disk.o fs.o fs_internal.o main.o
	gcc -o exefile initialize.o disk.o fs.o fs_internal.o main.o
initialize.o : initialize.c
	gcc -c -o initialize.o initialize.c
disk.o : disk.c
	gcc -c -o disk.o disk.c
fs.o : fs.c
	gcc -c -o fs.o fs.c
fs_internal.o : fs_internal.c
	gcc -c -o fs_internal.o fs_internal.c
main.o : main.c
	gcc -c -o main.o main.c
clean :
	rm *.o exefile MY_DISK
