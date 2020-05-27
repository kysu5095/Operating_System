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
    char* file_sys_info_block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    char* inode_bytemap_block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    char* block_bytemap_block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    char* inodelist_1_block   = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    char* inodelist_2_block   = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    char* inodelist_3_block   = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    char* inodelist_4_block   = (char*)malloc(sizeof(char) * BLOCK_SIZE);

    memset(file_sys_info_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(inode_bytemap_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(block_bytemap_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(inodelist_1_block,   '0', sizeof(char) * BLOCK_SIZE);
    memset(inodelist_2_block,   '0', sizeof(char) * BLOCK_SIZE);
    memset(inodelist_3_block,   '0', sizeof(char) * BLOCK_SIZE);
    memset(inodelist_4_block,   '0', sizeof(char) * BLOCK_SIZE);

    DevWriteBlock(0, file_sys_info_block);
    DevWriteBlock(1, inode_bytemap_block);
    DevWriteBlock(2, block_bytemap_block);
    DevWriteBlock(3, inodelist_1_block);
    DevWriteBlock(4, inodelist_2_block);
    DevWriteBlock(5, inodelist_3_block);
    DevWriteBlock(6, inodelist_4_block);
    for(int i = 7; i < BLOCK_SIZE; i++){
        char* block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
        memset(block, '0', sizeof(char) * BLOCK_SIZE);
        DevWriteBlock(i, block);
    }
}