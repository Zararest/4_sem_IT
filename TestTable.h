#ifndef TEST_HASHTABLE 
#define TEST_HASHTABLE
#include "HashTable.h"

#define MAX_KEY_SIZE 15

struct TableCell_{

    void* element;
    int elementSize;
    Key key;
    int valid;
};

struct HashTable_{

    TableCell* content;
    int size;
    int capacity;
    int deletedNum;
    float loadFactor;
    float dirtyFactor;
};

void rehash_(HashTable* table, int rehashFactor);
void insertCell_(HashTable* table, TableCell* cell);

int test_table(const char* fileName);
int tets_negatives();
#endif

