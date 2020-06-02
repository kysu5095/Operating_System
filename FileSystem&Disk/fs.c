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

int dirParsing(char** path, int cnt, int last, int* block_idx, DirEntry* dir, Inode* pInode, int flag){
    for(int idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        /* exist same directory name or file name */
        if(strcmp(path[cnt], dir[idx].name) == 0){
            /* termination condition */
            if(cnt == last - 1) {
                if(flag == 0){
                    perror("dirParsing : already exist directory name or file name");
                    return -1;
                }
                else 
                    return idx;
            } 
            /* into next block */
            GetInode(dir[idx].inodeNum, pInode);
            *block_idx = pInode->dirBlockPtr[0];
            DevReadBlock(*block_idx, (char*)dir);
            
            return dirParsing(path, cnt + 1, last, block_idx, dir, pInode, flag);
        }
        /* find last path */
        if(flag == 0 && cnt == last - 1 && idx == NUM_OF_DIRECT_BLOCK_PTR - 1)
            return 0;
    }
    return -1;
}

void freeMemory(char** memory, int cnt){
    for(int i = 0; i < cnt; i++)
        free(memory[i]);
    free(memory);
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
    if(dirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode, 0) == -1){
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
            /* ??? */
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
                if(pFileDesc[des].bUsed == 0){
                    pFileDesc[des].bUsed = 1;
                    pFileDesc[des].pOpenFile = (File*)malloc(sizeof(File));
                    pFileDesc[des].pOpenFile = file;
                    index = des;
                    break;
                }
            }
            break;
        }
    }

    /* memory release */
    freeMemory(pathArr, cnt);
    return index;
}

int	OpenFile(const char* szFileName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    int root_block_idx = pInode->dirBlockPtr[0];
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(root_block_idx, (char*)dir);

    /* path parsing */
    int cnt = getPathLen(szFileName);
    char** pathArr  = pathParsing(szFileName, &cnt);
    int inode_idx;
    if((inode_idx = dirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode, 1)) == -1){
        perror("OpenFile : no parent directory");
        return -1;
    }

    /* set file descriptor and file object */
    File* file = (File*)malloc(sizeof(File));
    file->inodeNum = inode_idx;
    file->fileOffset = 0;
    int index = -1;
    for(int des = 0; des < MAX_FD_ENTRY_MAX; des++){
        if(pFileDesc[des].bUsed == 0){
            pFileDesc[des].bUsed = 1;
            pFileDesc[des].pOpenFile = (File*)malloc(sizeof(File));
            pFileDesc[des].pOpenFile = file;
            index = des;
            break;
        }
        if(des == MAX_FD_ENTRY_MAX - 1){
            perror("OpenFile : file descriptor is full");
            return -1;
        }
    }

    /* memory release */
    freeMemory(pathArr, cnt);
    return index;
}

int	CloseFile(int fileDesc) {
    free(pFileDesc[fileDesc].pOpenFile);
    pFileDesc[fileDesc].bUsed = 0;
    pFileDesc[fileDesc].pOpenFile = NULL;
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
    int fd = pFileDesc[fileDesc].pOpenFile->inodeNum;
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(fd, pInode);
    for(int idx = 0; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        if(pInode->dirBlockPtr[idx] == 0){
            pInode->dirBlockPtr[idx] = block_idx;
            PutInode(fd, pInode);
            break;
        }
        if(idx == NUM_OF_DIRECT_BLOCK_PTR - 1){
            perror("WriteFile : full dirBlockPtr");
            return -1;
        }
    }

    /* write file */
    pFileDesc[fileDesc].pOpenFile->fileOffset += BLOCK_SIZE;
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    memset(block, 0, sizeof(BLOCK_SIZE));
    //////////////////////////////
    /*     block = pBuffer??    */
    //////////////////////////////
    strcpy(block, pBuffer);
    DevWriteBlock(block_idx, block);
    
    /* update block bytemap */
    SetBlockBytemap(block_idx);

    /* update file system information block */
    FileSysInfo* fileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
    fileSysInfo->numAllocBlocks++;
    fileSysInfo->numFreeBlocks--;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
    
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
    char* block = (char*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(pInode->dirBlockPtr[logical_block_idx], block);
    strcpy(pBuffer, block);

    /* update file descriptor */
    pFileDesc[fileDesc].pOpenFile->fileOffset += BLOCK_SIZE;

    return length;
}

int	RemoveFile(const char* szFileName) {
/* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    int root_block_idx = pInode->dirBlockPtr[0];
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(root_block_idx, (char*)dir);

    /* path parsing */
    int cnt = getPathLen(szFileName);
    char** pathArr = pathParsing(szFileName, &cnt);
    int entry_idx;
    if((entry_idx = dirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode, 1)) == -1){
        perror("RemoveFile : no parent directory");
        return -1;
    }

    /* delete file */
    GetInode(dir[entry_idx].inodeNum, pInode);
    int block_idx = pInode->dirBlockPtr[0];
    DevReadBlock(block_idx, (char*)dir);
    for(int idx = 2; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        if(strcmp("null", dir[idx].name) != 0){
            perror("RemoveFile : directory has child");
            return -1;
        }
    }

    /* reinitialize directory inode and update inode bytemap */
    memset(pInode, 0, sizeof(Inode));
    PutInode(dir[entry_idx].inodeNum, pInode);
    ResetInodeBytemap(dir[entry_idx].inodeNum);

    /* reinitialize directory block and update block bytemap */
    int parent_block_idx = dir[1].inodeNum;
    memset(dir, 0, sizeof(char) * BLOCK_SIZE);
    DevWriteBlock(block_idx, (char*)dir);
    ResetBlockBytemap(block_idx);

    /* update parent directory block data */
    DirEntry* parent_dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(parent_block_idx, (char*)parent_dir);
    strcpy(parent_dir[block_idx].name, "null");
    parent_dir[block_idx].inodeNum = 0;

    /* update file system information block */
    FileSysInfo* fileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
    fileSysInfo->numAllocBlocks--;
    fileSysInfo->numFreeBlocks++;
    fileSysInfo->numAllocInodes--;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);

    return 0;
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
    if(dirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode, 0) == -1){
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
    freeMemory(pathArr, cnt);
    return 0;
}

int	RemoveDir(const char* szDirName) {
    /* get root inode & get root block */
    Inode* pInode = (Inode*)malloc(sizeof(Inode));
    GetInode(0, pInode);
    int root_block_idx = pInode->dirBlockPtr[0];
    DirEntry* dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(root_block_idx, (char*)dir);

    /* path parsing */
    int cnt = getPathLen(szDirName);
    char** pathArr = pathParsing(szDirName, &cnt);
    int entry_idx;
    if((entry_idx = dirParsing(pathArr, 0, cnt, &root_block_idx, dir, pInode, 1)) == -1){
        perror("RemoveDir : no parent directory");
        return -1;
    }

    /* delete directory */
    GetInode(dir[entry_idx].inodeNum, pInode);
    int block_idx = pInode->dirBlockPtr[0];
    DevReadBlock(block_idx, (char*)dir);
    for(int idx = 2; idx < NUM_OF_DIRECT_BLOCK_PTR; idx++){
        if(strcmp("null", dir[idx].name) != 0){
            perror("RemoveDir : directory has child");
            return -1;
        }
    }

    /* reinitialize directory inode and update inode bytemap */
    memset(pInode, 0, sizeof(Inode));
    PutInode(dir[entry_idx].inodeNum, pInode);
    ResetInodeBytemap(dir[entry_idx].inodeNum);

    /* reinitialize directory block and update block bytemap */
    int parent_block_idx = dir[1].inodeNum;
    memset(dir, 0, sizeof(char) * BLOCK_SIZE);
    DevWriteBlock(block_idx, (char*)dir);
    ResetBlockBytemap(block_idx);

    /* update parent directory block data */
    DirEntry* parent_dir = (DirEntry*)malloc(sizeof(DirEntry) * NUM_OF_DIRENT_PER_BLOCK);
    DevReadBlock(parent_block_idx, (char*)parent_dir);
    strcpy(parent_dir[block_idx].name, "null");
    parent_dir[block_idx].inodeNum = 0;

    /* update file system information block */
    FileSysInfo* fileSysInfo = (FileSysInfo*)malloc(sizeof(BLOCK_SIZE));
    DevReadBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);
    fileSysInfo->numAllocBlocks--;
    fileSysInfo->numFreeBlocks++;
    fileSysInfo->numAllocInodes--;
    DevWriteBlock(FILESYS_INFO_BLOCK, (char*)fileSysInfo);

    return 0;
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
    DevReadBlock(0, (char*)pFileSysInfo);
}

void CloseFileSystem() {
    DevWriteBlock(0, (char*)pFileSysInfo);
}

int	GetFileStatus(const char* szPathName, FileStatus* pStatus)
{

}