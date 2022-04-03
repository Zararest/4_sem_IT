#ifndef HASHTABLE_ 
#define HASHTABLE_

#include <stddef.h>

#define MIN_CAPACITY 8
#define REHASH_FACTOR 2 
#define MAX_LOADFACTOR 0.5
#define MAX_DIRTYFACTOR 0.3                                

typedef char* Key;

typedef struct TableCell_ TableCell;
typedef struct HashTable_ HashTable;

HashTable* createHashTable(int capacity);
void rehash(HashTable* table);
void freeHashTable(HashTable* table);

int addElem(HashTable* table, Key key, void* element, int elementLen);
void* getElem(HashTable* table, Key key);
void removeElem(HashTable* table, Key key);

unsigned Hash(const char* element);

#endif