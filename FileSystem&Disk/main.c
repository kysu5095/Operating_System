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
	CreateFileSystem();
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
	DevReadBlock(7, (char*)dir);
	printf("%d : %s(%d)\n", dir[0].inodeNum, dir[0].name, (int)strlen(dir[0].name));
	return 0;
}
