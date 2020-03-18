#pragma once
#define HASH_TBL_SIZE 8
#define MAX_OBJLIST_NUM 3
#define OBJ_INVALID -1
#include <stddef.h>
#include <stdio.h>

typedef struct Object object;

struct Object {
	int objnum;
	Object* phPrev;
	Object* phNext;
};

typedef struct __HashTableEnt {
	int eletCount;
	Object* pHead;
	Object* pTail;
}HashTableEnt;

HashTableEnt pHashTableEnt[HASH_TBL_SIZE];

Object* pFreeListHead = NULL;
Object* ppFreeListTail = NULL;

void Init();
void InsertObjectToTail(Object* pObj, int ObjNum);
void InsertObjectToHead(Object* pObj, int objNum);
Object* GetObjectByNum(int objnum);
bool DeleteObject(Object* pObj);
void InsertObjectIntoObjFreeList(Object* pObj);
