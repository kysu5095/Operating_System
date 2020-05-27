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
    DevWriteBlock(7, (char*)dir);

    /* initial file system information block */
    pFileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
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

    /* setting file system block */
    pFileSysInfo->numAllocBlocks++;
    pFileSysInfo->numFreeBlocks--;
    pFileSysInfo->numAllocInodes++;
    DevWriteBlock(0, (char*)pFileSysInfo);

    // /* inode bytemap block */
    // char inode_bytemap[MAX_FD_ENTRY_MAX];
    // memset(inode_bytemap, 0, sizeof(inode_bytemap));
    // inode_bytemap[0] = '1';
    // memcpy(inode_bytemap_block, inode_bytemap, BLOCK_SIZE);

    // /* block bytemap block */
    // char block_bytemap[MAX_FD_ENTRY_MAX];
    // memset(block_bytemap, 0, sizeof(block_bytemap));
}


void OpenFileSystem() {

}


void CloseFileSystem() {

}
