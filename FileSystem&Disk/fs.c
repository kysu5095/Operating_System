#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"

int		CreateFile(const char* szFileName) {

}

int		OpenFile(const char* szFileName) {

}


int		WriteFile(int fileDesc, char* pBuffer, int length) {

}

int		ReadFile(int fileDesc, char* pBuffer, int length) {

}


int		CloseFile(int fileDesc) {

}

int		RemoveFile(const char* szFileName) {

}


int		MakeDir(const char* szDirName) {

}


int		RemoveDir(const char* szDirName) {

}

int   EnumerateDirStatus(const char* szDirName, DirEntryInfo* pDirEntry, int dirEntrys) {

}


void	CreateFileSystem() {
    int block_idx = GetFreeBlockNum();
    int inode_idx = GetFreeInodeNum();
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    strcpy(dir[0].name, ".");
    dir[0].inodeNum = 0;
    DevWriteBlock(7, (char*)dir);
}


void	OpenFileSystem() {

}


void	CloseFileSystem() {

}
