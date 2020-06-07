exe : initialize.o disk.o fs.o fs_internal.o testcase.o
	gcc -o exe initialize.o disk.o fs.o fs_internal.o testcase.o
initialize.o : initialize.c
	gcc -c -o initialize.o initialize.c
disk.o : disk.c
	gcc -c -o disk.o disk.c
fs.o : fs.c
	gcc -c -o fs.o fs.c
fs_internal.o : fs_internal.c
	gcc -c -o fs_internal.o fs_internal.c
testcase.o : testcase.c
	gcc -c -o testcase.o testcase.c
clean :
	rm *.o exe MY_DISK
