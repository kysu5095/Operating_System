#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
	printf("Hello OS World\n");
	CreateFileSystem();
	
	// char* block = (char*)malloc(BLOCK_SIZE);
	// DevReadBlock(0, block);
	// printf("%s\n", block);
    // DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
	// DevReadBlock(7, (char*)dir);
	// printf("%d : %s(%d)\n", dir[0].inodeNum, dir[0].name, (int)strlen(dir[0].name));
    // FileSysInfo* dir2 = (FileSysInfo*)malloc(sizeof(FileSysInfo) * NUM_OF_DIRENT_PER_BLOCK);
	// DevReadBlock(0, (char*)dir2);
	// printf("Blocks : %d\n", dir2->blocks);
	// printf("RootInodeNum : %d\n", dir2->rootInodeNum);
	// printf("DiskCapacity : %d\n", dir2->diskCapacity);
	// printf("NumAllocBlocks : %d\n", dir2->numAllocBlocks);
	// printf("NumFreeBlocks : %d\n", dir2->numFreeBlocks);
	// printf("NumAllocInodes : %d\n", dir2->numAllocInodes);
	// printf("BlockByteMapBlock : %d\n", dir2->blockBytemapBlock);
	// printf("InodeByteMapBlock : %d\n", dir2->inodeBytemapBlock);
	// printf("InodeListBlock : %d\n", dir2->inodeListBlock);
	// printf("DataRegionBlock : %d\n", dir2->dataRegionBlock);

	printf("directory : %d\n", NUM_OF_DIRENT_PER_BLOCK);
	printf("inode : %d\n", NUM_OF_INODE_PER_BLOCK);
	for(int i = 0; i < 50; i++){
		int a = i;
		char b[5];
		sprintf(b, "/tmp%d", a);
		printf("%s\n", b);
		MakeDir(b);
	}

	return 0;
}
