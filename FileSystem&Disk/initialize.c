#include "disk.h"
#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
//#include <io.h>
#include <unistd.h>

/* File System Initialize */ 
void FileSysInit(void){
    char* file_sys_info = (char*)malloc(BLOCK_SIZE);
    char* inode_bytemap = (char*)malloc(BLOCK_SIZE);
    char* block_bytemap = (char*)malloc(BLOCK_SIZE);
    char* inodelist_1   = (char*)malloc(BLOCK_SIZE);
    char* inodelist_2   = (char*)malloc(BLOCK_SIZE);
    char* inodelist_3   = (char*)malloc(BLOCK_SIZE);
    char* inodelist_4   = (char*)malloc(BLOCK_SIZE);

    memset(file_sys_info, 0, sizeof(file_sys_info));
    memset(inode_bytemap, 0, sizeof(inode_bytemap));
    memset(block_bytemap, 0, sizeof(block_bytemap));
    memset(inodelist_1,   0, sizeof(inodelist_1));
    memset(inodelist_2,   0, sizeof(inodelist_2));
    memset(inodelist_3,   0, sizeof(inodelist_3));
    memset(inodelist_4,   0, sizeof(inodelist_4));

    DevWriteBlock(0, file_sys_info);
    DevWriteBlock(1, inode_bytemap);
    DevWriteBlock(2, block_bytemap);
    DevWriteBlock(3, inodelist_1);
    DevWriteBlock(4, inodelist_2);
    DevWriteBlock(5, inodelist_3);
    DevWriteBlock(6, inodelist_4);

    // FileSysInfo* file_sys_info = (FileSysInfo*)malloc(sizeof(FileSysInfo));
    // file_sys_info->blocks = BLOCK_SIZE;
    // file_sys_info->rootInodeNum = 0;
    // file_sys_info->diskCapacity = BLOCK_SIZE * BLOCK_SIZE;
    // file_sys_info->numAllocBlocks = 0;
    // file_sys_info->numFreeBlocks = BLOCK_SIZE;
    // file_sys_info->numAllocInodes = 0;
    // file_sys_info->blockBytemapBlock = 2;
    // file_sys_info->inodeBytemapBlock = 1;
    // file_sys_info->inodeListBlock = 3;
    // file_sys_info->dataRegionBlock = 7;
    // DevWriteBlock(0, (char*)file_sys_info);
}