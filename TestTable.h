#ifndef TEST_HASHTABLE 
#define TEST_HASHTABLE
#include "HashTable.h"

struct _TableCell{

    char* element;
    Key key;
    Bool valid;
};

struct _HashTable{

    TableCell* content;
    int size;
    int capacity;
    float loadFactor;
};

int test_createTable();
int test_rehash();

int test_addElem();
int test_getElem();
int test_removeElem();

int tets_negatives();
#endif

