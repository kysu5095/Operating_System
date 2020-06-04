#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(){
	printf("Hello OS World\n");
	CreateFileSystem();
	
	// printf("Blocks : %d\n", pFileSysInfo->blocks);
	// printf("RootInodeNum : %d\n", pFileSysInfo->rootInodeNum);
	// printf("DiskCapacity : %d\n", pFileSysInfo->diskCapacity);
	// printf("NumAllocBlocks : %d\n", pFileSysInfo->numAllocBlocks);
	// printf("NumFreeBlocks : %d\n", pFileSysInfo->numFreeBlocks);
	// printf("NumAllocInodes : %d\n", pFileSysInfo->numAllocInodes);
	// printf("BlockByteMapBlock : %d\n", pFileSysInfo->blockBytemapBlock);
	// printf("InodeByteMapBlock : %d\n", pFileSysInfo->inodeBytemapBlock);
	// printf("InodeListBlock : %d\n", pFileSysInfo->inodeListBlock);
	// printf("DataRegionBlock : %d\n", pFileSysInfo->dataRegionBlock);

	DirEntry* ddir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
	char* b = (char*)malloc(100);
	for(int i = 0; i < 40; i++){
		int a = i;
		sprintf(b, "/tmp%d", a);
		//printf("%s\n", b);
		MakeDir(b);
		// DevReadBlock(7, (char*)ddir);
		// for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
		// 	if(strcmp(ddir[j].name, "null") == 0) break;
		// 	printf("%s ", ddir[j].name); 
		// }
		// printf("\n");
	}
	//free(ddir);
	printf("===========remove==========\n");
	for(int i = 0; i < 35; i++){
		int a = i;
		char b[5];
		sprintf(b, "/tmp%d", a);
		printf("%s\n", b);
		RemoveDir(b);
	}

	Inode* inode = (Inode*)malloc(sizeof(Inode));
	GetInode(0, inode);
	for(int i = 0; i <= 1; i++){
		int val = inode->dirBlockPtr[i];
		printf("DirPtr : %d\n", val);
		if(val != 0){
			DevReadBlock(val, (char*)ddir);
			for(int j = 0; j < NUM_OF_DIRENT_PER_BLOCK; j++){
				printf("%s(%d)\n", ddir[j].name, ddir[j].inodeNum);
			}
		}
	}

	// Inode* inode = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
	// DevReadBlock(0, (char*)inode);
	// for(int i = 0; i < NUM_OF_INODE_PER_BLOCK; i++){
	// 	inode[i].allocBlocks = i + 1;
	// 	inode[i].size = i + 100;
	// 	inode[i].type = i + 10000;
	// 	for(int j = 0; j < NUM_OF_DIRECT_BLOCK_PTR; j++)
	// 		inode[i].dirBlockPtr[j] = i + j + 1000000;
	// }
	// DevWriteBlock(0, (char*)inode);
	// free(inode);
	// Inode* inode2 = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
	// DevReadBlock(0, (char*)inode2);
	// for(int i = 0; i < NUM_OF_INODE_PER_BLOCK; i++){
	// 	printf("BLOCK : %d  SIZE : %d  TYPE : %d  ", inode2[i].allocBlocks, inode2[i].size, inode2[i].type);
	// 	for(int j = 0; j < NUM_OF_DIRECT_BLOCK_PTR; j++){
	// 		printf("  PTR%d : %d", j, inode2[i].dirBlockPtr[j]);
	// 	}
	// 	printf("\n");
	// }
	// free(inode2);
	// char* tmp = (char*)malloc(BLOCK_SIZE);
	// DevReadBlock(2, tmp);
	// printf("%s(%d)\n", tmp, (int)strlen(tmp));
	// for(int i = 0; i < 10; i++)
	// 	SetBlockBytemap(i);
	// DevReadBlock(2, tmp);
	// printf("%s(%d)\n", tmp, (int)strlen(tmp));
	// free(tmp);

	DevCloseDisk();
	printf("finish\n");
	return 0;
}
