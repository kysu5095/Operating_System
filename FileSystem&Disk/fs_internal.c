#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"

void SetInodeBytemap(int inodeno) {
    char* inode = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM, inode);
    inode[inodeno] = 1;
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM, inode);
    free(inode);
}


void ResetInodeBytemap(int inodeno) {
    char* inode = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    DevReadBlock(INODE_BYTEMAP_BLOCK_NUM, inode);
    inode[inodeno] = 0;
    DevWriteBlock(INODE_BYTEMAP_BLOCK_NUM, inode);
    free(inode);
}


void SetBlockBytemap(int blkno) {
    char* block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM, block);
    block[blkno] = 1;
    DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM, block);
    free(block);
}


void ResetBlockBytemap(int blkno) {
    char* block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    DevReadBlock(BLOCK_BYTEMAP_BLOCK_NUM, block);
    block[blkno] = 0;
    DevWriteBlock(BLOCK_BYTEMAP_BLOCK_NUM, block);
    free(block);
}


void PutInode(int inodeno, Inode* pInode) {
    Inode* inode = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    int block_idx = (inodeno / NUM_OF_INODE_PER_BLOCK) + 3;
    DevReadBlock(block_idx, (char*)inode);
    int node_idx = inodeno % NUM_OF_INODE_PER_BLOCK;
    memcpy(inode + node_idx, pInode, sizeof(Inode));
    DevWriteBlock(block_idx, (char*)inode);
    free(inode);
}


void GetInode(int inodeno, Inode* pInode) {
    Inode* inode = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    int block_idx = (inodeno / NUM_OF_INODE_PER_BLOCK) + 3;
    DevReadBlock(block_idx, (char*)inode);
    int node_idx = inodeno % NUM_OF_INODE_PER_BLOCK;
    memcpy(pInode, inode + node_idx, sizeof(Inode));
    free(inode);
}


int GetFreeInodeNum(void) {
    char* inode = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    DevReadBlock(1, inode);
    for(int i = 0; i < MAX_FD_ENTRY_MAX; i++)
        if(inode[i] == 0)
            return i;
    free(inode);
    return -1;
}


int GetFreeBlockNum(void) {
    char* block = (char*)malloc(sizeof(char) * BLOCK_SIZE);
    DevReadBlock(2, block);
    for(int i = 7; i < BLOCK_SIZE; i++)
        if(block[i] == 0)
            return i;
    free(block);
    return -1;
}