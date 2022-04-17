#ifndef HASHTABLE_ 
#define HASHTABLE_

#include <stddef.h>

#define MIN_CAPACITY 8
#define REHASH_FACTOR 2 
#define MAX_LOADFACTOR 0.5
#define MAX_DIRTYFACTOR 0.3 

enum TableErrors{
    
    Ok,
    BadAlloc,
    MemcpyException,

    CapacityCorrupted,
    SizeCorrupted,
    DeletedNumCorrupted,
    LoadFactorCorrupted,
    DirtyFactorCorrupted, 

    LoopedSearch,

    WrongParam
};

typedef char* Key;

typedef struct TableCell_ TableCell;
typedef struct HashTable_ HashTable;

HashTable* createHashTable(int capacity, int* err);
void rehash(HashTable* table, int* err);
void freeHashTable(HashTable* table, int* err);

int addElem(HashTable* table, Key key, void* element, int elementLen, int* err);
void* getElem(HashTable* table, Key key, int* err);
void removeElem(HashTable* table, Key key, int* err);

unsigned Hash(const char* element);

void what(int err);

#endif