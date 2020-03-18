#pragma once
#define HASH_TBL_SIZE 8
#define MAX_OBJLIST_NUM 3
#define OBJ_INVALID -1
#include <stddef.h>
#include <iostream>

typedef struct Object object;

struct Object {
	int objnum;
	object* phPrev;
	object* phNext;

	Object() {
		objnum = 0;
		phPrev = phNext = NULL;
	}
};

typedef struct __HashTableEnt {
	int elmtCount;
	object* pHead;
	object* pTail;
}HashTableEnt;

HashTableEnt pHashTableEnt[HASH_TBL_SIZE];

object* pFreeListHead = NULL;
object* pFreeListTail = NULL;

void Init();
void InsertObjectToTail(object* pObj, int ObjNum);
void InsertObjectToHead(object* pObj, int objNum);
object* GetObjectByNum(int objnum);
bool DeleteObject(object* pObj);
object* GetObjectFromObjFreeList(void);
void InsertObjectIntoObjFreeList(object* pObj);
