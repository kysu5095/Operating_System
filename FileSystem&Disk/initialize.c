#include "disk.h"
#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

FileSysInfo* pFileSysInfo;
void FileSysInit(void);

/* File System Initialize */ 
void FileSysInit(void){
    FileSysInfo* file_sys_info_block = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
    char* inode_bytemap_block = (char*) malloc(sizeof(char)  * BLOCK_SIZE);
    char* block_bytemap_block = (char*) malloc(sizeof(char)  * BLOCK_SIZE);
    Inode* inodelist_1_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    Inode* inodelist_2_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    Inode* inodelist_3_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    Inode* inodelist_4_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);

    memset(file_sys_info_block, 0, sizeof(BLOCK_SIZE));
    memset(inode_bytemap_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(block_bytemap_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(inodelist_1_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    memset(inodelist_2_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    memset(inodelist_3_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    memset(inodelist_4_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);

    DevWriteBlock(0, (char*)file_sys_info_block);
    DevWriteBlock(1, inode_bytemap_block);
    DevWriteBlock(2, block_bytemap_block);
    DevWriteBlock(3, (char*)inodelist_1_block);
    DevWriteBlock(4, (char*)inodelist_2_block);
    DevWriteBlock(5, (char*)inodelist_3_block);
    DevWriteBlock(6, (char*)inodelist_4_block);
    for(int i = 7; i < BLOCK_SIZE; i++){
        char* block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
        memset(block, '0', sizeof(char) * BLOCK_SIZE);
        DevWriteBlock(i, block);
    }

    /* file descriptor table initialize */
    for(int i = 0; i < MAX_FD_ENTRY_MAX; i++){
        fileDesc[i].bUsed = 0;
        fileDesc[i].pOpenFile = NULL;
    }
}