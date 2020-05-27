#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"

int	CreateFile(const char* szFileName) {

}

int	OpenFile(const char* szFileName) {

}


int	WriteFile(int fileDesc, char* pBuffer, int length) {

}

int	ReadFile(int fileDesc, char* pBuffer, int length) {

}


int	CloseFile(int fileDesc) {

}

int	RemoveFile(const char* szFileName) {

}


int	MakeDir(const char* szDirName) {
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    int root_block_idx = pInode->dirBlockPtr[0];
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(root_block_idx, (char*)dir);

    int idx;
    for(idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        if(strcmp(szDirName, dir[idx].name) == 0){
            perror("MakeDir : same directory name");
            return -1;
        }
    }
    for(idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++)
        if(strcmp("", dir[idx].name) == 0)
            break;
    
    int inode_idx;
    if((inode_idx = GetFreeInodeNum()) == -1){
        perror("MakeDir : inode_idx error");
        exit(0);
    }
    strcpy(dir[idx].name, szDirName);
    dir[idx].inodeNum = inode_idx;
}


int	RemoveDir(const char* szDirName) {

}

int EnumerateDirStatus(const char* szDirName, DirEntryInfo* pDirEntry, int dirEntrys) {

}


void CreateFileSystem() {
    /* get block, inode index */
    int block_idx, inode_idx;
    if((block_idx = GetFreeBlockNum()) == -1){
        perror("CreateFileSystem : block_idx error");
        exit(0);
    }
    if((inode_idx = GetFreeInodeNum()) == -1){
        perror("CreateFileSystem : inode_idx error");
        exit(0);
    }

    /* allocate DirEntry array to block size */
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    strcpy(dir[0].name, ".");
    dir[0].inodeNum = 0;
    for(int i = 1; i < NUM_OF_DIRENT_PER_BLOCK; i++){
        strcpy(dir[i].name, "null");
        dir[0].inodeNum = 0;
    }
    DevWriteBlock(block_idx, (char*)dir);

    /* initial file system information block */
    pFileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
    memset(pFileSysInfo, 0, sizeof(BLOCK_SIZE));
    pFileSysInfo->blocks            = BLOCK_SIZE;
    pFileSysInfo->rootInodeNum      = 0;
    pFileSysInfo->diskCapacity      = FS_DISK_CAPACITY;
    pFileSysInfo->numAllocBlocks    = 7;
    pFileSysInfo->numFreeBlocks     = BLOCK_SIZE - 7;    
    pFileSysInfo->numAllocInodes    = 0;
    pFileSysInfo->blockBytemapBlock = BLOCK_BYTEMAP_BLOCK_NUM;
    pFileSysInfo->inodeBytemapBlock = INODE_BYTEMAP_BLOCK_NUM;
    pFileSysInfo->inodeListBlock    = INODELIST_BLOCK_FIRST;
    pFileSysInfo->dataRegionBlock   = 7;

    /* update file system block */
    pFileSysInfo->numAllocBlocks++;
    pFileSysInfo->numFreeBlocks--;
    pFileSysInfo->numAllocInodes++;
    DevWriteBlock(0, (char*)pFileSysInfo);

    /* update block, inode bytemap */
    SetBlockBytemap(block_idx);
    SetInodeBytemap(inode_idx);

    /* setting inode */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    memset(pInode, 0, sizeof(Inode));
    GetInode(0, pInode);
    pInode->allocBlocks = 1;
    pInode->size = pInode->allocBlocks * 512;
    pInode->type = FILE_TYPE_DIR;
    pInode->dirBlockPtr[0] = block_idx;
    PutInode(0, pInode);
}


void OpenFileSystem() {

}


void CloseFileSystem() {

}
