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
    char*  tempPath = (char* )malloc((int)strlen(name) + 1);
    strcpy(tempPath, name);
    char*  ptr = strtok(tempPath, "/");
    for(int i = 0; i < (*cnt); i++){
        pathArr[i] = (char*)malloc((int)strlen(ptr) + 1);
        strcpy(pathArr[i], ptr);
        ptr = strtok(NULL, "/");
    }
    free(tempPath);
    return pathArr;
}

int dirParsing(char** path, int cnt, int last, int* inode_idx, int* block_idx, DirEntry* dir, Inode* pInode, int flag){
    int blkPtr;
    for(blkPtr = 0; blkPtr < NUM_OF_DIRECT_BLOCK_PTR; blkPtr++){
        if(pInode->dirBlockPtr[blkPtr] == 0) continue;
        DevReadBlock(pInode->dirBlockPtr[blkPtr], (char*)dir);
        for(int idx = 0; idx < NUM_OF_DIRENT_PER_BLOCK; idx++){
            /* exist same directory name or file name */
            if(strcmp(path[cnt], dir[idx].name) == 0){
                /* termination condition */
                if(cnt == last - 1) {
                    if(flag == 0){
                        perror("dirParsing : already exist directory name or file name");
                        return -1;
                    }
                    else {
                        *block_idx = pInode->dirBlockPtr[blkPtr];
                        return idx;
                    }
                } 
                /* into next block */
                GetInode(dir[idx].inodeNum, pInode);
                *inode_idx = dir[idx].inodeNum;
                if(pInode->type != FILE_TYPE_DIR){
                    perror("dirParsing : this is not directory");
                    return -1;
                }
                return dirParsing(path, cnt + 1, last, inode_idx, block_idx, dir, pInode, flag);
            }
        }
    }
    /* find last path */
    if(flag == 0 && cnt == last - 1 && blkPtr == NUM_OF_DIRECT_BLOCK_PTR)
        return 0;

    return -1;
}

void getPtrIndex(Inode* pInode, DirEntry* dir, int* inode_idx, int* block_ptr, int* idx){
    for(int ptr = 0; ptr < NUM_OF_DIRECT_BLOCK_PTR; ptr++){
        if(pInode->dirBlockPtr[ptr] == 0){
            /* get block index */
            int block_idx;
            if((block_idx = GetFreeBlockNum()) == -1){
                perror("getPtrIndex : block_idx error");
                return; 
            }
            /* make new block ptr */
            DevReadBlock(block_idx, (char*)dir);
            strcpy(dir[0].name, ".");
            dir[0].inodeNum = *inode_idx;
            if(*inode_idx == 0){
                strcpy(dir[1].name, "null");
                dir[1].inodeNum = 0;
            }
            else{
                strcpy(dir[1].name, "..");
                DirEntry* parentDir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
                DevReadBlock(pInode->dirBlockPtr[0], (char*)parentDir);
                dir[1].inodeNum = parentDir[0].inodeNum;
                free(parentDir);
            }
            for(int i = 2; i < NUM_OF_DIRENT_PER_BLOCK; i++){
                strcpy(dir[i].name, "null");
                dir[i].inodeNum = 0;
            }
            DevWriteBlock(block_idx, (char*)dir);

            /* update inode, file */
            pInode->allocBlocks++;
            pInode->size = pInode->allocBlocks * 512;
            pInode->dirBlockPtr[ptr] = block_idx;
            PutInode(*inode_idx, pInode);

            /* update block bytemap */
            SetBlockBytemap(block_idx);

            /* update file system information */
            DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
            pFileSysInfo->numAllocBlocks++;
            pFileSysInfo->numFreeBlocks--;
            DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
            
            *block_ptr = ptr;
            *idx = 1;
            return;
        }
        else{
            DevReadBlock(pInode->dirBlockPtr[ptr], (char*)dir);
            for(int i = 0; i < NUM_OF_DIRENT_PER_BLOCK; i++){
                if(strcmp(dir[i].name, "null") == 0){
                    *block_ptr = ptr;
                    *idx = i;
                    return;
                }
            }
        }
    }
    return;
}

void freeMemory(char** memory, int cnt){
    for(int i = 0; i < cnt; i++)
        free(memory[i]);
    free(memory);
}

int	CreateFile(const char* szFileName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int idx, parent_block_idx = 0, parent_inode_idx = 0, blockPtr;

    /* path parsing */
    int cnt = getPathLen(szFileName);
    char** pathArr  = pathParsing(szFileName, &cnt);
    if(dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 0) == -1){
        perror("CreateFile : no parent directory");
        return -1;
    }
    getPtrIndex(pInode, dir, &parent_inode_idx, &blockPtr, &idx);

    /* get inode index */
    int inode_idx, index = -1;
    if((inode_idx = GetFreeInodeNum()) == -1){
        perror("CreateFile : inode_idx error");
        return -1;
    }

    /* make new file */
    strcpy(dir[idx].name, pathArr[cnt - 1]);
    dir[idx].inodeNum = inode_idx;
    DevWriteBlock(pInode->dirBlockPtr[blockPtr], (char*)dir);

    /* setting inode */
    GetInode(inode_idx, pInode);
    memset(pInode, 0, sizeof(Inode));
    pInode->allocBlocks = 0;
    pInode->size = 0;
    pInode->type = FILE_TYPE_FILE;
    PutInode(inode_idx, pInode);

    /* update inode bytemap */
    SetInodeBytemap(inode_idx);

    /* update file system information block */
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
    pFileSysInfo->numAllocInodes++;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

    /* set file descriptor and file object */
    File* file = (File*)malloc(sizeof(File));
    file->inodeNum = inode_idx;
    file->fileOffset = 0;
    for(int des = 0; des < MAX_FD_ENTRY_MAX; des++){
        if(pFileDesc[des].bUsed == 0){
            pFileDesc[des].bUsed = 1;
            pFileDesc[des].pOpenFile = (File*)malloc(sizeof(File));
            memcpy(pFileDesc[des].pOpenFile, file, sizeof(File));
            index = des;
            break;
        }
    }

    /* memory release */
    free(pInode);
    free(dir);
    free(file);
    freeMemory(pathArr, cnt);
    return index;
}

int	OpenFile(const char* szFileName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int entry_idx, parent_inode_idx = 0, parent_block_idx;

    /* path parsing */
    int cnt = getPathLen(szFileName);
    char** pathArr = pathParsing(szFileName, &cnt);
    if((entry_idx = dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 1)) == -1){
        perror("OpenFile : no parent directory");
        return -1;
    }

    /* set file descriptor and file object */
    File* file = (File*)malloc(sizeof(File));
    file->inodeNum = dir[entry_idx].inodeNum;
    file->fileOffset = 0;
    int index = -1;
    for(int des = 0; des < MAX_FD_ENTRY_MAX; des++){
        if(pFileDesc[des].bUsed == 0){
            pFileDesc[des].bUsed = 1;
            pFileDesc[des].pOpenFile = (File*)malloc(sizeof(File));
            memcpy(pFileDesc[des].pOpenFile, file, sizeof(File));
            index = des;
            break;
        }
    }

    /* memory release */
    free(pInode);
    free(dir);
    free(file);
    freeMemory(pathArr, cnt);
    return index;
}

int	CloseFile(int fileDesc) {
    free(pFileDesc[fileDesc].pOpenFile);
    pFileDesc[fileDesc].bUsed = 0;
    pFileDesc[fileDesc].pOpenFile = NULL;
}

int WriteFileFunc(char* pBuffer, int length, int block_idx, int dirBlock, Inode* pInode, File* file){
    char* block = (char*)malloc(BLOCK_SIZE);
    int offset = (file->fileOffset) % BLOCK_SIZE;
    DevReadBlock(block_idx, block);
    memcpy(block + offset, pBuffer, length);
    DevWriteBlock(block_idx, block);
    pInode->dirBlockPtr[dirBlock] = block_idx;
    file->fileOffset += length;
    free(block);
}

void UpdateBytemapAndFileSysInfo(int block_idx){
    /* update block bytemap */
    SetBlockBytemap(block_idx);   

    /* update file system information block */
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
    pFileSysInfo->numAllocBlocks++;
    pFileSysInfo->numFreeBlocks--;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
}

int	WriteFile(int fileDesc, char* pBuffer, int length) {
    /* get block index */
    int block_idx;
    if((block_idx = GetFreeBlockNum()) == -1){
        perror("WriteFile : block_idx error");
        return -1;
    }

    /* get file descriptor */
    if(pFileDesc[fileDesc].bUsed == 0){
        perror("WriteFile : get file descriptor error");
        return -1;
    }
    File* file = pFileDesc[fileDesc].pOpenFile;
    int fd = file->inodeNum;
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(fd, pInode);
    char* block = (char*)malloc(BLOCK_SIZE);
    int logical_block_idx = (file->fileOffset) / BLOCK_SIZE;
    
    /* use one block */
    if(length <= BLOCK_SIZE){
        /* create dirBlockPtr */
        if(pInode->dirBlockPtr[logical_block_idx] == 0){
            pInode->allocBlocks++;
            pInode->size = pInode->allocBlocks * BLOCK_SIZE;
            UpdateBytemapAndFileSysInfo(block_idx);     
        }
        /* overwrite */
        else block_idx = pInode->dirBlockPtr[logical_block_idx];
        WriteFileFunc(pBuffer, length, block_idx, logical_block_idx, pInode, file);
    }
    /* use two or more block */
    else{
        int length_1 = BLOCK_SIZE;
        int length_2 = length_1 - BLOCK_SIZE;

        /* create dirBlockPtr */
        if(pInode->dirBlockPtr[logical_block_idx] == 0){
            pInode->allocBlocks++;
            pInode->size = pInode->allocBlocks * BLOCK_SIZE;
            UpdateBytemapAndFileSysInfo(block_idx);     
        }
        /* overwrite */
        else block_idx = pInode->dirBlockPtr[logical_block_idx];
        WriteFileFunc(pBuffer, length_1, block_idx, logical_block_idx, pInode, file);
        
        /* get additional block index */
        int block_idx2;
        if((block_idx2 = GetFreeBlockNum()) == -1){
            perror("WriteFile : block_idx error");
            return -1;
        }
        logical_block_idx++;
        /* create dirBlockPtr */
        if(pInode->dirBlockPtr[logical_block_idx] == 0){
            pInode->allocBlocks++;
            pInode->size = pInode->allocBlocks * BLOCK_SIZE;
            UpdateBytemapAndFileSysInfo(block_idx2);     
        }
        /* overwrite */
        else block_idx2 = pInode->dirBlockPtr[logical_block_idx];
        WriteFileFunc(pBuffer + BLOCK_SIZE, length_2, block_idx2, logical_block_idx, pInode, file);
    }

    /* write inode */
    PutInode(fd, pInode);
    
    free(pInode);
    free(block);
    return length;
}

int	ReadFile(int fileDesc, char* pBuffer, int length) {
    if(pFileDesc[fileDesc].bUsed == 0){
        perror("ReadFile : get file descriptor error");
        return -1;
    }

    /* get inode & get block*/
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(pFileDesc[fileDesc].pOpenFile->inodeNum, pInode);
    int logical_block_idx = (pFileDesc[fileDesc].pOpenFile->fileOffset) / BLOCK_SIZE;
    char* block = (char*)malloc(BLOCK_SIZE);
    DevReadBlock(pInode->dirBlockPtr[logical_block_idx], (char*)block);
    memcpy(pBuffer, block, length);
    DevWriteBlock(pInode->dirBlockPtr[logical_block_idx], (char*)block);
    /* update file descriptor */
    pFileDesc[fileDesc].pOpenFile->fileOffset += BLOCK_SIZE;

    free(pInode);
    free(block);
    return length;
}

int	RemoveFile(const char* szFileName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int entry_idx, parent_inode_idx = 0, parent_block_idx;

    /* path parsing */
    int cnt = getPathLen(szFileName);
    char** pathArr = pathParsing(szFileName, &cnt);
    if((entry_idx = dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 1)) == -1){
        perror("RemoveDir : no parent directory");
        return -1;
    }

    /* check the file is open */
    for(int i = 0; i < MAX_FD_ENTRY_MAX; i++){
        if(pFileDesc[i].bUsed == 0) continue;
        if(pFileDesc[i].pOpenFile->inodeNum == dir[entry_idx].inodeNum){
            perror("RemoveFile : file is open");
            return -1;
        }
    }

    /* delete file */
    /* reinitialize directory inode and update inode bytemap */
    GetInode(dir[entry_idx].inodeNum, pInode);
    for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++){
        if(pInode->dirBlockPtr[i] == 0) continue;
        char* block = (char*)malloc(BLOCK_SIZE);
        DevReadBlock(pInode->dirBlockPtr[i], block);
        memset(block, 0, BLOCK_SIZE);
        DevWriteBlock(pInode->dirBlockPtr[i], block);
        ResetBlockBytemap(pInode->dirBlockPtr[i]);

        /* update file system information block */
        DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
        pFileSysInfo->numAllocBlocks--;
        pFileSysInfo->numFreeBlocks++;
        DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

        free(block);
    }
    memset(pInode, 0, sizeof(Inode));
    PutInode(dir[entry_idx].inodeNum, pInode);
    ResetInodeBytemap(dir[entry_idx].inodeNum);

    /* update parent directory block data */
    strcpy(dir[entry_idx].name, "null");
    dir[entry_idx].inodeNum = 0;
    DevWriteBlock(parent_block_idx, (char*)dir);

    /* update file system information block */
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
    pFileSysInfo->numAllocInodes--;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

    free(pInode);
    free(dir);
    return 0;
}

int	MakeDir(const char* szDirName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int idx, parent_block_idx = 0, parent_inode_idx = 0, blockPtr;
    /* path parsing */
    int cnt = getPathLen(szDirName);
    char** pathArr = pathParsing(szDirName, &cnt);
    if(dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 0) == -1){
        perror("MakeDir : no parent directory");
        return -1;
    }
    getPtrIndex(pInode, dir, &parent_inode_idx, &blockPtr, &idx);

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

    /* update parent directory */
    strcpy(dir[idx].name, pathArr[cnt - 1]);
    dir[idx].inodeNum = inode_idx;
    DevWriteBlock(pInode->dirBlockPtr[blockPtr], (char*)dir);

    /* make new directory */
    DirEntry* newDir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(block_idx, (char*)newDir);    
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

    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////
    /* increase capacity of the all parent directory inode */
    /////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////

    /* update block, inode bytemap */
    SetInodeBytemap(inode_idx);
    SetBlockBytemap(block_idx);

    /* update file system information block */
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
    pFileSysInfo->numAllocBlocks++;
    pFileSysInfo->numFreeBlocks--;
    pFileSysInfo->numAllocInodes++;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

    /* memory release */
    free(dir);
    free(newDir);
    freeMemory(pathArr, cnt);
    return 0;
}

int	RemoveDir(const char* szDirName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int idx, parent_inode_idx = 0;

    /* path parsing */
    int cnt = getPathLen(szDirName);
    char** pathArr = pathParsing(szDirName, &cnt);
    int entry_idx, parent_block_idx;
    if((entry_idx = dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 1)) == -1){
        perror("RemoveDir : no parent directory");
        return -1;
    }

    /* delete directory */
    DirEntry* childDir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(parent_block_idx, (char*)childDir);

    GetInode(childDir[entry_idx].inodeNum, pInode);
    int block_idx;
    for(int ptr = 0; ptr < NUM_OF_DIRECT_BLOCK_PTR; ptr++){
        if(pInode->dirBlockPtr[ptr] == 0) continue;
        block_idx = pInode->dirBlockPtr[ptr];
        DevReadBlock(block_idx, (char*)childDir);
        for(int idx = 2; idx < NUM_OF_DIRENT_PER_BLOCK; idx++){
            if(strcmp("null", childDir[idx].name) != 0){
                perror("RemoveDir : directory has child");
                return -1;
            }
        }
    }

    for(int ptr = 0; ptr < NUM_OF_DIRECT_BLOCK_PTR; ptr++){
        if(pInode->dirBlockPtr[ptr] == 0) continue;
        block_idx = pInode->dirBlockPtr[ptr];
        DevReadBlock(block_idx, (char*)childDir);
        memset(childDir, 0, sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
        DevWriteBlock(block_idx, (char*)childDir);
        ResetBlockBytemap(block_idx);

        pInode->dirBlockPtr[ptr] = 0;
        DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
        pFileSysInfo->numAllocBlocks--;
        pFileSysInfo->numFreeBlocks++;
        DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
    }

    /* decrease capacity of the all parent directory inode*/
    Inode* parentInode = (Inode*)malloc(sizeof(Inode));
    GetInode(parent_inode_idx, parentInode);
    parentInode->allocBlocks -= pInode->allocBlocks;
    parentInode->size = parentInode->allocBlocks * 512;
    PutInode(parent_inode_idx, parentInode);

    /* reinitialize directory inode and update inode bytemap */
    GetInode(dir[entry_idx].inodeNum, pInode);
    memset(pInode, 0, sizeof(Inode));
    PutInode(dir[entry_idx].inodeNum, pInode);
    ResetInodeBytemap(dir[entry_idx].inodeNum);

    DevReadBlock(parent_block_idx, (char*)dir);
    strcpy(dir[entry_idx].name, "null");
    dir[entry_idx].inodeNum = 0;
    DevWriteBlock(parent_block_idx, (char*)dir);

    DevReadBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);
    pFileSysInfo->numAllocInodes--;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

    free(dir);
    free(childDir);
    free(pInode);
    free(parentInode);
    return 0;
}

int EnumerateDirStatus(const char* szDirName, DirEntryInfo* pDirEntry, int dirEntrys) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int idx, parent_block_idx = 0, parent_inode_idx = 0, blockPtr;

    /* path parsing */
    int cnt = getPathLen(szDirName);
    char** pathArr = pathParsing(szDirName, &cnt);
    int entry_idx;
    if((entry_idx = dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 1)) == -1){
        perror("EnumerateDirStatus : no parent directory");
        return -1;
    }

    int count = 0;
    Inode* childeInode = (Inode*)malloc(sizeof(Inode));
    /* into directory */
    GetInode(dir[entry_idx].inodeNum, pInode);
    for(int ptr = 0; ptr < NUM_OF_DIRECT_BLOCK_PTR; ptr++){
        if(count == dirEntrys) break;
        if(pInode->dirBlockPtr[ptr] == 0) continue;
        DevReadBlock(pInode->dirBlockPtr[ptr], (char*)dir);
        for(int i = 2; i < NUM_OF_DIRENT_PER_BLOCK; i++){
            if(strcmp(dir[i].name, "null") == 0) continue;
            memcpy(pDirEntry[count].name, dir[i].name, sizeof(dir[i].name));
            pDirEntry[count].inodeNum = dir[i].inodeNum;
            GetInode(dir[i].inodeNum, childeInode);
            pDirEntry[count].type = childeInode->type;
            count++;
            if(count == dirEntrys) break;
        }
    }

    free(pInode);
    free(dir);
    free(childeInode);
    return count;
}

void CreateFileSystem() {
    /* create file system */
    DevCreateDisk();
    FileSysInit();
    
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
        dir[i].inodeNum = 0;
    }
    DevWriteBlock(block_idx, (char*)dir);
    free(dir);

    /* initial file system information block */
    pFileSysInfo = (FileSysInfo*)malloc(BLOCK_SIZE);
    memset(pFileSysInfo, 0, BLOCK_SIZE);
    pFileSysInfo->blocks            = BLOCK_SIZE;
    pFileSysInfo->rootInodeNum      = inode_idx;
    pFileSysInfo->diskCapacity      = FS_DISK_CAPACITY;
    pFileSysInfo->numAllocBlocks    = 6;
    pFileSysInfo->numFreeBlocks     = BLOCK_SIZE - 6;    
    pFileSysInfo->numAllocInodes    = 0;
    pFileSysInfo->blockBytemapBlock = BLOCK_BYTEMAP_BLOCK_NUM;
    pFileSysInfo->inodeBytemapBlock = INODE_BYTEMAP_BLOCK_NUM;
    pFileSysInfo->inodeListBlock    = INODELIST_BLOCK_FIRST;
    pFileSysInfo->dataRegionBlock   = 7;

    /* update file system information block */
    pFileSysInfo->numAllocBlocks++;
    pFileSysInfo->numFreeBlocks--;
    pFileSysInfo->numAllocInodes++;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)pFileSysInfo);

    /* update block, inode bytemap */
    SetBlockBytemap(block_idx);
    SetInodeBytemap(inode_idx);

    /* setting inode */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    memset(pInode, 0, sizeof(Inode));
    pInode->allocBlocks = 1;
    pInode->size = pInode->allocBlocks * 512;
    pInode->type = FILE_TYPE_DIR;
    pInode->dirBlockPtr[0] = block_idx;
    PutInode(0, pInode);
    free(pInode);
}

void OpenFileSystem() {
    DevOpenDisk();
    pFileSysInfo = (FileSysInfo*)malloc(BLOCK_SIZE);
    memset(pFileSysInfo, 0, BLOCK_SIZE);
    DevReadBlock(0, (char*)pFileSysInfo);
}

void CloseFileSystem() {
    DevWriteBlock(0, (char*)pFileSysInfo);
    DevCloseDisk();
}

int	GetFileStatus(const char* szPathName, FileStatus* pStatus){
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    int entry_idx, parent_inode_idx = 0, parent_block_idx;

    /* path parsing */
    int cnt = getPathLen(szPathName);
    char** pathArr = pathParsing(szPathName, &cnt);
    if((entry_idx = dirParsing(pathArr, 0, cnt, &parent_inode_idx, &parent_block_idx, dir, pInode, 1)) == -1){
        perror("GetFileStatus : no parent directory");
        return -1;
    }

    /* copy data */
    GetInode(dir[entry_idx].inodeNum, pInode);
    pStatus->allocBlocks = pInode->allocBlocks;
    pStatus->size = pInode->size;
    pStatus->type = pInode->type;
    for(int i = 0; i < NUM_OF_DIRECT_BLOCK_PTR; i++)
        pStatus->dirBlockPtr[i] = pInode->dirBlockPtr[i];
    free(pInode);
    free(dir);
    return 0;
}