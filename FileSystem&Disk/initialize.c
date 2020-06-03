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
    pFileSysInfo = (FileSysInfo*)malloc(BLOCK_SIZE);
    char* inode_bytemap_block = (char*) malloc(sizeof(char)  * BLOCK_SIZE);
    char* block_bytemap_block = (char*) malloc(sizeof(char)  * BLOCK_SIZE);
    Inode* inodelist_1_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    Inode* inodelist_2_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    Inode* inodelist_3_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    Inode* inodelist_4_block  = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);

    memset(pFileSysInfo, 0, BLOCK_SIZE);
    memset(inode_bytemap_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(block_bytemap_block, '0', sizeof(char) * BLOCK_SIZE);
    memset(inodelist_1_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    memset(inodelist_2_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    memset(inodelist_3_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    memset(inodelist_4_block, 0, sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);

    DevWriteBlock(0, (char*)pFileSysInfo);
    DevWriteBlock(1, inode_bytemap_block);
    DevWriteBlock(2, block_bytemap_block);
    DevWriteBlock(3, (char*)inodelist_1_block);
    DevWriteBlock(4, (char*)inodelist_2_block);
    DevWriteBlock(5, (char*)inodelist_3_block);
    DevWriteBlock(6, (char*)inodelist_4_block);

    free(inode_bytemap_block);
    free(block_bytemap_block);
    free(inodelist_1_block);
    free(inodelist_2_block);
    free(inodelist_3_block);
    free(inodelist_4_block);

    for(int i = 0; i < 7; i++)
        SetBlockBytemap(i);

    for(int i = 7; i < BLOCK_SIZE; i++){
        char* block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
        memset(block, '0', sizeof(char) * BLOCK_SIZE);
        DevWriteBlock(i, block);
        free(block);
    }

    /* file descriptor table initialize */
    for(int i = 0; i < MAX_FD_ENTRY_MAX; i++){
        pFileDesc[i].bUsed = 0;
        pFileDesc[i].pOpenFile = NULL;
    }
}