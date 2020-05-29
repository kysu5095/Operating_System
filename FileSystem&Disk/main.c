#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
	printf("Hello OS World\n");
	DevCreateDisk();
	FileSysInit();
	CreateFileSystem();
	/*// char* block = (char*)malloc(BLOCK_SIZE);
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
	
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(0, inode);
	printf("AllocBlocks : %d\n", inode->allocBlocks);
	printf("Size : %d\n", inode->size);
	printf("Type : %d\n", inode->type);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[0]);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[1]);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[2]);*/

	MakeDir("/tmp");
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(1, inode);
	printf("AllocBlocks : %d\n", inode->allocBlocks);
	printf("Size : %d\n", inode->size);
	printf("Type : %d\n", inode->type);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[0]);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[1]);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[2]);
	return 0;
}
