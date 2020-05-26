#include <stdio.h>
#include <stdlib.h>
#include "disk.h"
#include "fs.h"

void SetInodeBytemap(int inodeno) {
    char* inode = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(1, inode);
    inode[inodeno] = 1;
    DevWriteBlock(1, inode);
}


void ResetInodeBytemap(int inodeno) {
    char* inode = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(1, inode);
    inode[inodeno] = 0;
    DevWriteBlock(1, inode);
}


void SetBlockBytemap(int blkno) {
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(2, block);
    block[blkno] = 1;
    DevWriteBlock(2, block);
}


void ResetBlockBytemap(int blkno) {
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(2, block);
    block[blkno] = 0;
    DevWriteBlock(2, block);
}


void PutInode(int inodeno, Inode* pInode) {
    char* inode_list = (char*)malloc(sizeof(BLOCK_SIZE));
    int block_idx = (inodeno / NUM_OF_INODE_PER_BLOCK) + 3;
    DevReadBlock(block_idx, inode_list);
    int node_idx = inodeno % NUM_OF_INODE_PER_BLOCK;
    memcpy(inode_list + (node_idx * sizeof(Inode)), pInode, sizeof(Inode));
}


void GetInode(int inodeno, Inode* pInode) {
    char* inode_list = (char*)malloc(sizeof(BLOCK_SIZE));
    int block_idx = (inodeno / NUM_OF_INODE_PER_BLOCK) + 3;
    DevReadBlock(block_idx, inode_list);
    int node_idx = inodeno % NUM_OF_INODE_PER_BLOCK;
    memcpy(pInode, inode_list + (node_idx * sizeof(Inode)), sizeof(Inode));
}


int GetFreeInodeNum(void) {
    char* inode = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(1, inode);
    for(int i = 0; i < MAX_FD_ENTRY_MAX; i++)
        if(inode[i] == '0')
            return i;
    
    return -1;
}


int GetFreeBlockNum(void) {
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(2, block);
    for(int i = 0; i < MAX_FD_ENTRY_MAX; i++)
        if(block[i] == '0')
            return i;
    
    return -1;
}