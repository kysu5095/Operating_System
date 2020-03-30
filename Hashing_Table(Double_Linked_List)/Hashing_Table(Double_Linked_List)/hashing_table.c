#include "hashing_table.h"
#include <malloc.h>
#include <stdlib.h>

#define INSERT_OBJECT_SIZE (48)

void InitFreeList() {

	int i;
	Object* pObject;

	for (i = 0; i < INSERT_OBJECT_SIZE; i++) {
		pObject = (Object*)malloc(sizeof(Object));
		pObject->objnum = OBJ_INVALID;
		InsertObjectIntoObjFreeList(pObject);
	}
}

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
	/*pFreeListHead = NULL;
	ppFreeListTail = NULL;*/
}

// Insert object to **Tail** of hash table
void InsertObjectToTail(Object* pObj, int ObjNum) {
	int table_num = ObjNum % HASH_TBL_SIZE;
	pObj->objnum = ObjNum;
	if (pHashTableEnt[table_num].pHead == NULL) {
		pHashTableEnt[table_num].pHead = pObj;
		pHashTableEnt[table_num].pHead->phPrev = NULL;
		pHashTableEnt[table_num].pHead->phNext = NULL;
	}
	else {
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
	if (pHashTableEnt[table_num].pHead == NULL) {
		pHashTableEnt[table_num].pHead = pObj;
		pHashTableEnt[table_num].pHead->phPrev = NULL;
		pHashTableEnt[table_num].pHead->phNext = NULL;
	}
	else {
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
	Object *tmp = (Object*)malloc(sizeof(Object));
	for (int i = 0; i < HASH_TBL_SIZE; i++) {
		tmp = pHashTableEnt[i].pHead;
		while (tmp != NULL) {
			if (tmp == pObj) {
				// When delete head
				if (tmp == pHashTableEnt[i].pHead) {
					// When hash table size is 1
					if (tmp->phNext == NULL) {
						pHashTableEnt[i].pHead = NULL;
						pHashTableEnt[i].pTail = NULL;
					}
					else {
						// When hash table size is 2
						if (tmp->phNext == pHashTableEnt[i].pTail) {
							pHashTableEnt[i].pHead = pHashTableEnt[i].pTail;
							pHashTableEnt[i].pHead->phNext = NULL;
							pHashTableEnt[i].pHead->phPrev = NULL;
							pHashTableEnt[i].pTail = NULL;
						}
						else {
							pHashTableEnt[i].pHead = pHashTableEnt[i].pHead->phNext;
							pHashTableEnt[i].pHead->phPrev = NULL;
						}
					}
				}
				else {
					// When delete tail
					if (tmp == pHashTableEnt[i].pTail) {
						// When hash table size is 2
						if (tmp->phPrev == pHashTableEnt[i].pHead) {
							pHashTableEnt[i].pHead->phNext = NULL;
							pHashTableEnt[i].pTail = NULL;
						}
						else {
							pHashTableEnt[i].pTail = pHashTableEnt[i].pTail->phPrev;
							pHashTableEnt[i].pTail->phNext = NULL;
						}
					}
					else {
						tmp->phPrev->phNext = tmp->phNext;
						tmp->phNext->phPrev = tmp->phPrev;
					}
				}
				
				//// When didn't delete head
				//if (pHashTableEnt[i].elmtCount > 1) {
				//	// When didn't delete tail
				//	if (pHashTableEnt[i].elmtCount == 2) {
				//		pHashTableEnt[i].pHead->phNext = NULL;
				//		pHashTableEnt[i].pTail->phPrev = NULL;
				//	}
				//	else {
				//		tmp->phPrev->phNext = tmp->phNext;
				//		tmp->phNext->phPrev = tmp->phPrev;
				//	}
				//}
				pHashTableEnt[i].elmtCount--;
				pObj = (Object *)malloc(sizeof(Object));
				pObj->objnum = OBJ_INVALID;
				//delete tmp;
				return 1;
			}
			if (tmp->phNext == NULL) break;
			tmp = tmp->phNext;
		}
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//void print_hash() {
//	printf("--------------------HASH TABLE--------------------\n");
//	for (int i = 0; i < HASH_TBL_SIZE; i++) {
//		printf("%d : ", i);
//		Object* tmp = pHashTableEnt[i].pHead->phNext;
//		while (tmp != pHashTableEnt[i].pTail) {
//			printf("%d(%p)  ", tmp->objnum, tmp);
//			tmp = tmp->phNext;
//		}
//		printf("\n");
//	}
//	printf("--------------------------------------------------\n");
//}
//
//void print_free_list() {
//	int i = 1;
//	printf("--------------------FREE LIST---------------------\n");
//	Object* tmp = pFreeListHead->phNext;
//	while (tmp != ppFreeListTail) {
//		printf("%d %p \n", i++, tmp);
//		tmp = tmp->phNext;
//	}
//	printf("\n--------------------------------------------------\n");
//}

void printHashTable() {
	int i, j;
	printf("-----------HashTable-----------\n");
	for (i = 0; i < HASH_TBL_SIZE; i++) {
		printf("index: %d |", i);
		Object* temp = pHashTableEnt[i].pHead;
		for (j = 0; j < pHashTableEnt[i].elmtCount; j++) {
			printf(" %d", temp->objnum);
			temp = temp->phNext;
		}
		printf("\n");
	}
}

void DeleteProcess(int idx) {
	Object* temp = GetObjectByNum(idx);
	temp->objnum = OBJ_INVALID;
	DeleteObject(temp);
	InsertObjectIntoObjFreeList(temp);
}

void main() {

	int i;

	Init();
	InitFreeList();

	//testcase1
	for (i = 0; i < INSERT_OBJECT_SIZE; i++) {
		if (i % 2 == 0)
			InsertObjectToTail(GetObjectFromObjFreeList(), i);
		else
			InsertObjectToHead(GetObjectFromObjFreeList(), i);
	}

	printf("case1) Insert Input\n");
	//print_hash();
	printHashTable();

	for (i = 0; i < INSERT_OBJECT_SIZE; i++) {
		DeleteProcess(i);
	}

	printf("case1) Delete All\n");
	printHashTable();

	//testcase2
	for (i = 0; i < INSERT_OBJECT_SIZE; i++) {
		InsertObjectToTail(GetObjectFromObjFreeList(), i);
	}

	for (i = 8; i < 16; i++) {
		DeleteProcess(i);
		InsertObjectToHead(GetObjectFromObjFreeList(), i);
	}
	for (i = 32; i < 40; i++) {
		DeleteProcess(i);
		InsertObjectToHead(GetObjectFromObjFreeList(), i);
	}
	printf("case2)\n");
	printHashTable();
	
	return 0;
}

//int main() {
//	Object* pObject;
//	Init();
//	for (int i = 0; i < 10; i++) {
//		pObject = (Object*)malloc(sizeof(Object));
//		pObject->objnum = 1;
//		InsertObjectIntoObjFreeList(pObject);
//	}
//	print_free_list();
//
//	pObject = GetObjectFromObjFreeList();
//	pObject->objnum = 10;
//	InsertObjectToTail(pObject, 10);
//
//	pObject = GetObjectFromObjFreeList();
//	pObject->objnum = 1;
//	InsertObjectToTail(pObject, 1);
//
//	pObject = GetObjectFromObjFreeList();
//	pObject->objnum = 2;
//	InsertObjectToTail(pObject, 2);
//
//	pObject = GetObjectFromObjFreeList();
//	pObject->objnum = 5;
//	InsertObjectToTail(pObject, 5);
//
//	pObject = GetObjectFromObjFreeList();
//	pObject->objnum = 18;
//	InsertObjectToHead(pObject, 18);
//
//	print_free_list();
//	printf("\n");
//	print_hash();
//
//	pObject = GetObjectByNum(5);
//	DeleteObject(pObject);
//	pObject->objnum = 1;
//	InsertObjectIntoObjFreeList(pObject);
//
//	print_free_list();
//	printf("\n");
//	print_hash();
//	return 0;
//}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
