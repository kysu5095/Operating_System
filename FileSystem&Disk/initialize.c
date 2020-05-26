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

}