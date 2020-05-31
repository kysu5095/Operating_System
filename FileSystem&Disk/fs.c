#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "fs.h"

int getPathLen(const char* szDirName){
    int cnt = 0;
    for(int i = 0; i < (int)strlen(szDirName); i++)
        if(szDirName[i] == '/')
            cnt++;
    return cnt;
}

char** pathParsing(const char* name, int* cnt){
    char** pathArr  = (char**)malloc(sizeof(char*) * (*cnt));
    char*  tempPath = (char* )malloc(sizeof((int)strlen(name)));
    strcpy(tempPath, name);
    char*  ptr = strtok(tempPath, "/");
    for(int i = 0; i < (*cnt); i++){
        int len = (int)strlen(ptr);
        pathArr[i] = (char*)malloc(sizeof(char) * len);
        strcpy(pathArr[i], ptr);
        ptr = strtok(NULL, "/");
    }
    free(ptr);
    return pathArr;
}

int DirParsing(char** path, int cnt, int last, int* block_idx, DirEntry* dir, Inode* pInode){
    for(int idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        /* exist same directory name or file name */
        if(strcmp(path[cnt], dir[idx].name) == 0){
            /* termination condition */
            if(cnt == last - 1) {
                perror("DirParsing : already exist directory name or file name");
                return -1;
            } 
            /* into next block */
            GetInode(dir[idx].inodeNum, pInode);
            *block_idx = pInode->dirBlockPtr[0];
            DevReadBlock(*block_idx, (char*)dir);
            
            return DirParsing(path, cnt + 1, last, block_idx, dir, pInode);
        }
        /* find last path */
        if(cnt == last - 1 && idx == NUM_OF_DIRECT_BLOCK_PTR - 1)
            return 0;
    }
    return -1;
}

int	CreateFile(const char* szFileName) {
    /* get inode index */
    int inode_idx, index = -1;
    if((inode_idx = GetFreeInodeNum()) == -1){
        perror("CreateFile : inode_idx error");
        return -1;
    }

    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    int root_block_idx = pInode->dirBlockPtr[0];
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(root_block_idx, (char*)dir);

    /* path parsing */
    int cnt = getPathLen(szFileName);
    char** pathArr  = pathParsing(szFileName, &cnt);
    if(DirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode) == -1){
        perror("CreateFile : no parent directory");
        return -1;
    }

    /* make new file */
    for(int idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        /* find empty entry */
        if(strcmp("null", dir[idx].name) == 0){
            strcpy(dir[idx].name, pathArr[cnt - 1]);
            dir[idx].inodeNum = inode_idx;
            DevWriteBlock(root_block_idx, (char*)dir);

            /* setting inode */
            GetInode(inode_idx, pInode);
            memset(pInode, 0, sizeof(Inode));
            pInode->allocBlocks = 0;
            pInode->size = 0;
            pInode->type = FILE_TYPE_FILE;
            /* 뭐지 */
            // pInode->dirBlockPtr[0] = block_idx;
            PutInode(inode_idx, pInode);

            /* update inode bytemap */
            SetInodeBytemap(inode_idx);

            /* update file system information block */
            FileSysInfo* fileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
            DevReadBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
            fileSysInfo->numAllocInodes++;
            DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
            
            /* set file descriptor and file object */
            File* file = (File*)malloc(sizeof(File));
            file->inodeNum = inode_idx;
            file->fileOffset = 0;
            for(int des = 0; des < MAX_FD_ENTRY_MAX; des++){
                if(fileDesc[des].bUsed == 0){
                    fileDesc[des].bUsed = 1;
                    fileDesc[des].pOpenFile = file;
                    index = des;
                    break;
                }
            }
            break;
        }
    }

    /* memory release */
    for(int i = 0; i < cnt; i++)
        free(pathArr[i]);
    free(pathArr);
    return index;
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
    /* get block, inode index */
    int block_idx, inode_idx;
    if((block_idx = GetFreeBlockNum()) == -1){
        perror("MakeDir : block_idx error");
        return -1;
    }
    if((inode_idx = GetFreeInodeNum()) == -1){
        perror("MakeDir : inode_idx error");
        return -1;
    }

    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    int root_block_idx = pInode->dirBlockPtr[0];
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(root_block_idx, (char*)dir);

    /* path parsing */
    int cnt = getPathLen(szDirName);
    char** pathArr = pathParsing(szDirName, &cnt);
    if(DirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode) == -1){
        perror("MakeDir : no parent directory");
        return -1;
    }

    /* make new directory */
    for(int idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        /* find empty entry */
        if(strcmp("null", dir[idx].name) == 0){
            strcpy(dir[idx].name, pathArr[cnt - 1]);
            dir[idx].inodeNum = inode_idx;
            DevWriteBlock(root_block_idx, (char*)dir);

            /* new directory block */
            DirEntry* newDir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
            strcpy(newDir[0].name, ".");
            newDir[0].inodeNum = inode_idx;
            strcpy(newDir[1].name, "..");
            newDir[1].inodeNum = dir[0].inodeNum;
            for(int i = 2; i < NUM_OF_DIRENT_PER_BLOCK; i++){
                strcpy(newDir[i].name, "null");
                newDir[i].inodeNum = 0;
            }
            DevWriteBlock(block_idx, (char*)newDir);

            /* setting inode */
            GetInode(inode_idx, pInode);
            memset(pInode, 0, sizeof(Inode));
            pInode->allocBlocks = 1;
            pInode->size = pInode->allocBlocks * 512;
            pInode->type = FILE_TYPE_DIR;
            pInode->dirBlockPtr[0] = block_idx;
            PutInode(inode_idx, pInode);

            /* update block, inode bytemap */
            SetBlockBytemap(block_idx);
            SetInodeBytemap(inode_idx);

            /* update file system information block */
            FileSysInfo* fileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
            DevReadBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
            fileSysInfo->numAllocBlocks++;
            fileSysInfo->numFreeBlocks--;
            fileSysInfo->numAllocInodes++;
            DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
            break;
        }
    }

    /* memory release */
    for(int i = 0; i < cnt; i++)
        free(pathArr[i]);
    free(pathArr);
    return 0;
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
    dir[0].inodeNum = inode_idx;
    for(int i = 1; i < NUM_OF_DIRENT_PER_BLOCK; i++){
        strcpy(dir[i].name, "null");
        dir[0].inodeNum = 0;
    }
    DevWriteBlock(block_idx, (char*)dir);

    /* initial file system information block */
    pFileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
    memset(pFileSysInfo, 0, sizeof(BLOCK_SIZE));
    pFileSysInfo->blocks            = BLOCK_SIZE;
    pFileSysInfo->rootInodeNum      = inode_idx;
    pFileSysInfo->diskCapacity      = FS_DISK_CAPACITY;
    pFileSysInfo->numAllocBlocks    = 7;
    pFileSysInfo->numFreeBlocks     = BLOCK_SIZE - 7;    
    pFileSysInfo->numAllocInodes    = 0;
    pFileSysInfo->blockBytemapBlock = BLOCK_BYTEMAP_BLOCK_NUM;
    pFileSysInfo->inodeBytemapBlock = INODE_BYTEMAP_BLOCK_NUM;
    pFileSysInfo->inodeListBlock    = INODELIST_BLOCK_FIRST;
    pFileSysInfo->dataRegionBlock   = 7;

    /* update file system information block */
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
