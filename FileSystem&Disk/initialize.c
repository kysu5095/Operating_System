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

FileSysInfo* pFileSysInfo;
void FileSysInit(void);

/* File System Initialize */ 
void FileSysInit(void){
    char* file_sys_info_block = (char*)malloc(BLOCK_SIZE);
    char* inode_bytemap_block = (char*)malloc(BLOCK_SIZE);
    char* block_bytemap_block = (char*)malloc(BLOCK_SIZE);
    char* inodelist_1_block   = (char*)malloc(BLOCK_SIZE);
    char* inodelist_2_block   = (char*)malloc(BLOCK_SIZE);
    char* inodelist_3_block   = (char*)malloc(BLOCK_SIZE);
    char* inodelist_4_block   = (char*)malloc(BLOCK_SIZE);

    memset(file_sys_info_block, 0, sizeof(BLOCK_SIZE));
    memset(inode_bytemap_block, 0, sizeof(BLOCK_SIZE));
    memset(block_bytemap_block, 0, sizeof(BLOCK_SIZE));
    memset(inodelist_1_block,   0, sizeof(BLOCK_SIZE));
    memset(inodelist_2_block,   0, sizeof(BLOCK_SIZE));
    memset(inodelist_3_block,   0, sizeof(BLOCK_SIZE));
    memset(inodelist_4_block,   0, sizeof(BLOCK_SIZE));

    // /* file system information block */
    // pFileSysInfo = (FileSysInfo*)malloc(sizeof(FileSysInfo));
    // pFileSysInfo->blocks            = BLOCK_SIZE;
    // pFileSysInfo->rootInodeNum      = 0;
    // pFileSysInfo->diskCapacity      = FS_DISK_CAPACITY;
    // pFileSysInfo->numAllocBlocks    = 7;
    // pFileSysInfo->numFreeBlocks     = BLOCK_SIZE - 7;    
    // pFileSysInfo->numAllocInodes    = 1;
    // pFileSysInfo->blockBytemapBlock = BLOCK_BYTEMAP_BLOCK_NUM;
    // pFileSysInfo->inodeBytemapBlock = INODE_BYTEMAP_BLOCK_NUM;
    // pFileSysInfo->inodeListBlock    = INODELIST_BLOCK_FIRST;
    // pFileSysInfo->dataRegionBlock   = 7;
    // memcpy(file_sys_info_block, pFileSysInfo, BLOCK_SIZE);

    // /* inode bytemap block */
    // char inode_bytemap[MAX_FD_ENTRY_MAX];
    // memset(inode_bytemap, 0, sizeof(inode_bytemap));
    // inode_bytemap[0] = '1';
    // memcpy(inode_bytemap_block, inode_bytemap, BLOCK_SIZE);

    // /* block bytemap block */
    // char block_bytemap[MAX_FD_ENTRY_MAX];
    // memset(block_bytemap, 0, sizeof(block_bytemap));

    DevWriteBlock(0, file_sys_info_block);
    DevWriteBlock(1, inode_bytemap_block);
    DevWriteBlock(2, block_bytemap_block);
    DevWriteBlock(3, inodelist_1_block);
    DevWriteBlock(4, inodelist_2_block);
    DevWriteBlock(5, inodelist_3_block);
    DevWriteBlock(6, inodelist_4_block);
}