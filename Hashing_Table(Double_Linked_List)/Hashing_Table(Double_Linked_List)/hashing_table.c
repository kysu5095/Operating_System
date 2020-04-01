#include "hashing_table.h"
#include <malloc.h>
#include <stdlib.h>

#define INSERT_OBJECT_SIZE (48)

// Initial Hash Table
void Init() {
	for (int i = 0; i < HASH_TBL_SIZE; i++) {
		pHashTableEnt[i].elmtCount = 0;
		pHashTableEnt[i].pHead = NULL;
		pHashTableEnt[i].pTail = NULL;
	}
	pFreeListHead = (Object*)malloc(sizeof(Object));
	ppFreeListTail = (Object*)malloc(sizeof(Object));
	pFreeListHead->phNext = ppFreeListTail;
	pFreeListHead->phPrev = NULL;
	ppFreeListTail->phNext = NULL;
	ppFreeListTail->phPrev = pFreeListHead;
}

// Insert object to **Tail** of hash table
void InsertObjectToTail(Object* pObj, int ObjNum) {
	int table_num = ObjNum % HASH_TBL_SIZE;
	pObj->objnum = ObjNum;
	// When hash table empty
	if (pHashTableEnt[table_num].pHead == NULL) {
		pHashTableEnt[table_num].pHead = pObj;
		pHashTableEnt[table_num].pHead->phPrev = NULL;
		pHashTableEnt[table_num].pHead->phNext = NULL;
	}
	else {
		// When hash table have only head
		if (pHashTableEnt[table_num].pTail == NULL) {
			pHashTableEnt[table_num].pTail = pObj;
			pHashTableEnt[table_num].pTail->phNext = NULL;
			pHashTableEnt[table_num].pTail->phPrev = pHashTableEnt[table_num].pHead;
			pHashTableEnt[table_num].pHead->phNext = pHashTableEnt[table_num].pTail;
		}
		else {
			Object* tmp = (Object*)malloc(sizeof(Object));
			tmp->objnum = pHashTableEnt[table_num].pTail->objnum;
			tmp->phPrev = pHashTableEnt[table_num].pTail->phPrev;
			pHashTableEnt[table_num].pTail->phPrev->phNext = tmp;
			pHashTableEnt[table_num].pTail = pObj;
			pHashTableEnt[table_num].pTail->phPrev = tmp;
			pHashTableEnt[table_num].pTail->phNext = NULL;
			tmp->phNext = pHashTableEnt[table_num].pTail;
		}
	}
	pHashTableEnt[table_num].elmtCount++;
}

// Insert object to **Head** of hash table
void InsertObjectToHead(Object* pObj, int ObjNum) {
	int table_num = ObjNum % HASH_TBL_SIZE;
	pObj->objnum = ObjNum;
	// When hash table is empty
	if (pHashTableEnt[table_num].pHead == NULL) {
		pHashTableEnt[table_num].pHead = pObj;
		pHashTableEnt[table_num].pHead->phPrev = NULL;
		pHashTableEnt[table_num].pHead->phNext = NULL;
	}
	else {
		// When hash table have only head
		if (pHashTableEnt[table_num].pTail == NULL) {
			pHashTableEnt[table_num].pTail = (Object*)malloc(sizeof(Object));
			pHashTableEnt[table_num].pTail->objnum = pHashTableEnt[table_num].pHead->objnum;
			pHashTableEnt[table_num].pHead = pObj;
			pHashTableEnt[table_num].pHead->phPrev = NULL;
			pHashTableEnt[table_num].pHead->phNext = pHashTableEnt[table_num].pTail;
			pHashTableEnt[table_num].pTail->phPrev = pHashTableEnt[table_num].pHead;
			pHashTableEnt[table_num].pTail->phNext = NULL;
		}
		else {
			Object* tmp = (Object*)malloc(sizeof(Object));
			tmp->objnum = pHashTableEnt[table_num].pHead->objnum;
			tmp->phNext = pHashTableEnt[table_num].pHead->phNext;
			pHashTableEnt[table_num].pHead->phNext->phPrev = tmp;
			pHashTableEnt[table_num].pHead = pObj;
			pHashTableEnt[table_num].pHead->phNext = tmp;
			tmp->phPrev = pHashTableEnt[table_num].pHead;
		}
	}
	pHashTableEnt[table_num].elmtCount++;
}

// Returns pointer of the found object
Object* GetObjectByNum(int objnum) {
	int table_num = objnum % HASH_TBL_SIZE;
	Object *tmp = (Object*)malloc(sizeof(Object));
	tmp = pHashTableEnt[table_num].pHead;
	while (1) {
		if (tmp->objnum == objnum) return tmp;
		if (tmp->phNext == NULL) break;
		tmp = tmp->phNext;
	}
	//delete tmp;
	return NULL;
}

// Delete object from hash table
BOOL DeleteObject(Object* pObj) {
	int table_num = pObj->objnum % HASH_TBL_SIZE;
	Object *tmp = (Object*)malloc(sizeof(Object));
	tmp = pHashTableEnt[table_num].pHead;
	while (tmp != NULL) {
		if (tmp == pObj) {
			// When delete head
			if (tmp == pHashTableEnt[table_num].pHead) {
				// When hash table size is 1
				if (tmp->phNext == NULL) {
					pHashTableEnt[table_num].pHead = NULL;
					pHashTableEnt[table_num].pTail = NULL;
				}
				else {
					// When hash table size is 2
					if (tmp->phNext == pHashTableEnt[table_num].pTail) {
						pHashTableEnt[table_num].pHead = pHashTableEnt[table_num].pTail;
						pHashTableEnt[table_num].pHead->phNext = NULL;
						pHashTableEnt[table_num].pHead->phPrev = NULL;
						pHashTableEnt[table_num].pTail = NULL;
					}
					else {
						pHashTableEnt[table_num].pHead = pHashTableEnt[table_num].pHead->phNext;
						pHashTableEnt[table_num].pHead->phPrev = NULL;
					}
				}
			}
			else {
				// When delete tail
				if (tmp == pHashTableEnt[table_num].pTail) {
					// When hash table size is 2
					if (tmp->phPrev == pHashTableEnt[table_num].pHead) {
						pHashTableEnt[table_num].pHead->phNext = NULL;
						pHashTableEnt[table_num].pTail = NULL;
					}
					else {
						pHashTableEnt[table_num].pTail = pHashTableEnt[table_num].pTail->phPrev;
						pHashTableEnt[table_num].pTail->phNext = NULL;
					}
				}
				else {
					tmp->phPrev->phNext = tmp->phNext;
					tmp->phNext->phPrev = tmp->phPrev;
				}
			}
			pHashTableEnt[table_num].elmtCount--;
			pObj = (Object *)malloc(sizeof(Object));
			pObj->objnum = OBJ_INVALID;
			//delete tmp;
			return 1;
		}
		if (tmp->phNext == NULL) break;
		tmp = tmp->phNext;
	}
	//delete tmp;
	return 0;
}

// Get new object from free list
Object* GetObjectFromObjFreeList(void) {
	if (ppFreeListTail->phPrev == pFreeListHead)
		return NULL;

	Object* tmp = ppFreeListTail->phPrev;
	ppFreeListTail->phPrev->phPrev->phNext = ppFreeListTail;
	ppFreeListTail->phPrev = ppFreeListTail->phPrev->phPrev;
	return tmp;
}

// Insert object to head of free list
void InsertObjectIntoObjFreeList(Object* pObj) {
	pFreeListHead->phNext->phPrev = pObj;
	pObj->phNext = pFreeListHead->phNext;
	pObj->phPrev = pFreeListHead;
	pFreeListHead->phNext = pObj;
}