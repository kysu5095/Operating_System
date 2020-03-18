#include "hashing_table.h"

// Initial Hash Table
void Init() {
	for (int i = 0; i < HASH_TBL_SIZE; i++) {
		pHashTableEnt[i].elmtCount = 0;
		pHashTableEnt[i].pHead = new Object();
		pHashTableEnt[i].pTail = new Object();
		pHashTableEnt[i].pHead->phNext = pHashTableEnt[i].pTail;
		pHashTableEnt[i].pTail->phPrev = pHashTableEnt[i].pHead;
	}
	pFreeListHead = new Object();
	pFreeListTail = new Object();
	pFreeListHead->phNext = pFreeListTail;
	pFreeListTail->phNext = pFreeListHead;
	pFreeListTail->phPrev = pFreeListHead;
}

// Insert object to **Tail** of hash table
void InsertObjectToTail(object* pObj, int ObjNum) {
	int table_num = ObjNum % HASH_TBL_SIZE;
	pHashTableEnt[table_num].pTail->phPrev->phNext = pObj;
	pObj->phPrev = pHashTableEnt[table_num].pTail->phPrev;
	pObj->phNext = pHashTableEnt[table_num].pTail;
	pHashTableEnt[table_num].pTail->phPrev = pObj;
	pHashTableEnt[table_num].elmtCount++;
	pObj->objnum = ObjNum;
}

// Insert object to **Head** of hash table
void InsertObjectToHead(object* pObj, int objNum) {
	int table_num = objNum % HASH_TBL_SIZE;
	pHashTableEnt[table_num].pHead->phNext->phPrev = pObj;
	pObj->phNext = pHashTableEnt[table_num].pHead->phNext;
	pObj->phPrev = pHashTableEnt[table_num].pHead;
	pHashTableEnt[table_num].pHead->phNext = pObj;
	pHashTableEnt[table_num].elmtCount++;
	pObj->objnum = objNum;
}

// Returns pointer of the found object
object* GetObjectByNum(int objnum) {
	int table_num = objnum % HASH_TBL_SIZE;
	object *tmp = new Object();
	tmp = pHashTableEnt[table_num].pHead->phNext;
	while (tmp->phNext != NULL) {
		if (tmp->objnum == objnum)
			return tmp;
		tmp = tmp->phNext;
	}
	delete tmp;
	return NULL;
}

// Delete object from hash table
bool DeleteObject(object* pObj) {
	object *tmp = new Object();
	for (int i = 0; i < HASH_TBL_SIZE; i++) {
		tmp = pHashTableEnt[i].pHead->phNext;
		while (tmp->phNext != NULL) {
			if (tmp == pObj) {
				tmp->phPrev->phNext = tmp->phNext;
				tmp->phNext->phPrev = tmp->phPrev;
				return true;
			}
			tmp = tmp->phNext;
		}
	}
	delete tmp;
	return false;
}

// Get new object from free list
object* GetObjectFromObjFreeList(void) {
	if (pFreeListTail->phPrev == pFreeListHead) 
		return NULL;

	object* tmp = pFreeListTail->phPrev;
	pFreeListTail->phPrev->phPrev->phNext = pFreeListTail;
	pFreeListTail->phPrev = pFreeListTail->phPrev->phPrev;
	return tmp;
}

// Insert object to head of free list
void InsertObjectIntoObjFreeList(object* pObj) {
	pFreeListHead->phNext->phPrev = pObj;
	pObj->phNext = pFreeListHead->phNext;
	pFreeListHead->phNext = pObj;
}

