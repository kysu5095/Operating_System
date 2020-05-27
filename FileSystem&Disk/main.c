#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
	printf("Hello OS World\n");
	DevCreateDisk();
	FileSysInit();
	// char* block = (char*)malloc(BLOCK_SIZE);
	// DevReadBlock(0, block);
	// printf("%s\n", block);
	CreateFileSystem();
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
	DevReadBlock(7, (char*)dir);
	printf("%d : %s(%d)\n", dir[0].inodeNum, dir[0].name, (int)strlen(dir[0].name));
    FileSysInfo* dir2 = (FileSysInfo*)malloc(sizeof(FileSysInfo) * NUM_OF_DIRENT_PER_BLOCK);
	DevReadBlock(0, (char*)dir2);
	//printf("block : %d\n rootInodeNum : %d\n diskCapacity : %d\n numAllocBlocks : %d\n numFreeBlocks : %d\n numAllocInodes : %d\n blockBytemapBlock : %d\n inodeBytemapBlock : %d\n inodeListBlock : %d\n dataRegionBlock\n", dir2->blocks, dir2->rootInodeNum, dir2->diskCapacity, dir2->numAllocBlocks, dir2->numFreeBlocks, dir2->numAllocInodes, dir2->blockBytemapBlock, dir2->inodeBytemapBlock, dir2->inodeListBlock, dir2->dataRegionBlock);
	printf("Blocks : %d\n", dir2->blocks);
	printf("RootInodeNum : %d\n", dir2->rootInodeNum);
	printf("DiskCapacity : %d\n", dir2->diskCapacity);
	printf("NumAllocBlocks : %d\n", dir2->numAllocBlocks);
	printf("NumFreeBlocks : %d\n", dir2->numFreeBlocks);
	printf("NumAllocInodes : %d\n", dir2->numAllocInodes);
	printf("BlockByteMapBlock : %d\n", dir2->blockBytemapBlock);
	printf("InodeByteMapBlock : %d\n", dir2->inodeBytemapBlock);
	printf("InodeListBlock : %d\n", dir2->inodeListBlock);
	printf("DataRegionBlock : %d\n", dir2->dataRegionBlock);
	
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	printf("\nBEFORE\n");
	printf("AllocBlocks : %d\n", inode->allocBlocks);
	printf("Size : %d\n", inode->size);
	printf("Type : %d\n", inode->type);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[0]);
	GetInode(0, inode);
	printf("\nAFTER\n");
	printf("AllocBlocks : %d\n", inode->allocBlocks);
	printf("Size : %d\n", inode->size);
	printf("Type : %d\n", inode->type);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[0]);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[1]);
	printf("DirBlockPtr : %d\n", inode->dirBlockPtr[2]);

	return 0;
}
