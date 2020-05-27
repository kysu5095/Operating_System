#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
	printf("Hello OS World\n");
	DevCreateDisk();
	FileSysInit();
	char* block = (char*)malloc(BLOCK_SIZE);
	DevReadBlock(0, block);
	printf("%s\n", block);
	return 0;
}
