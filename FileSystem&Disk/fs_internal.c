#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"

void SetInodeBytemap(int inodeno) {
    char* inode = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(1, inode);
    inode[inodeno] = '1';
    DevWriteBlock(1, inode);
}


void ResetInodeBytemap(int inodeno) {
    char* inode = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(1, inode);
    inode[inodeno] = '0';
    DevWriteBlock(1, inode);
}


void SetBlockBytemap(int blkno) {
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(2, block);
    block[blkno] = '1';
    DevWriteBlock(2, block);
}


void ResetBlockBytemap(int blkno) {
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(2, block);
    block[blkno] = '0';
    DevWriteBlock(2, block);
}


void PutInode(int inodeno, Inode* pInode) {
    Inode* inode = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    int block_idx = (inodeno / NUM_OF_INODE_PER_BLOCK) + 3;
    DevReadBlock(block_idx, (char*)inode);

    int node_idx = inodeno % NUM_OF_INODE_PER_BLOCK;
    inode[node_idx].allocBlocks = pInode->allocBlocks;
    inode[node_idx].size = pInode->size;
    inode[node_idx].type = pInode->type;
    for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++)
        inode[node_idx].dirBlockPtr[i] = pInode->dirBlockPtr[i];
    DevWriteBlock(block_idx, (char*)inode);
}


void GetInode(int inodeno, Inode* pInode) {
    Inode* inode = (Inode*)malloc(sizeof(Inode) * NUM_OF_INODE_PER_BLOCK);
    int block_idx = (inodeno / NUM_OF_INODE_PER_BLOCK) + 3;
    DevReadBlock(block_idx, (char*)inode);

    int node_idx = inodeno % NUM_OF_INODE_PER_BLOCK;
    pInode->allocBlocks = inode[node_idx].allocBlocks;
    pInode->size = inode[node_idx].size;
    pInode->type = inode[node_idx].type;
    for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++)
        pInode->dirBlockPtr[i] = inode[node_idx].dirBlockPtr[i];
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
    for(int i = 7; i < MAX_FD_ENTRY_MAX; i++)
        if(block[i] == '0')
            return i;
    
    return -1;
}