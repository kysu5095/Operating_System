#include "disk.h"
#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
//#include <io.h>
#include <unistd.h>

/* File System Initialize */ 
void FileSysInit(void){
    FileSysInfo* file_sys_info = (FileSysInfo*)malloc(sizeof(FileSysInfo));
    file_sys_info->blocks = BLOCK_SIZE;
    file_sys_info->rootInodeNum = 0;
    file_sys_info->diskCapacity = BLOCK_SIZE * BLOCK_SIZE;
    file_sys_info->numAllocBlocks = 0;
    file_sys_info->numFreeBlocks = BLOCK_SIZE;
    file_sys_info->numAllocInodes = 0;
    file_sys_info->blockBytemapBlock = 2;
    file_sys_info->inodeBytemapBlock = 1;
    file_sys_info->inodeListBlock = 3;
    file_sys_info->dataRegionBlock = 7;
    DevWriteBlock(0, (char*)file_sys_info);
}