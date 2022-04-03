#include "HashTable.h"
    
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>

struct TableCell_{//ключ это строка. Данные это void*. Add elem ничего не возвращает

    void* element;
    int elementSize;
    Key key;
    int valid; //-1 удален. 0 нету. 1 есть
};

struct HashTable_{

    TableCell* content;
    int size;
    int capacity;
    int deletedNum;
    float loadFactor;
    float dirtyFactor;
};

#define FIND_ELEM(errorHandle)  do{                                                                                     \
                                    while (curCell->valid == -1 || (curCell->valid == 1 && cmpKeys(curCell, key) == 0)){\
                                                                                                                        \
                                        curCell = table->content + (initialPos + shift) % table->capacity;              \
                                        shift++;                                                                        \
                                                                                                                        \
                                        if (shift == table->capacity){                                                  \
                                                                                                                        \
                                            fprintf(stderr, "Error: cyclic search: table corrupted\n");                 \
                                            errorHandle;                                                                \
                                        }                                                                               \
                                    }                                                                                   \
                                } while (0)

#define INIT_TABLE(capacitySize)do{                                 \
                                    table->capacity = capacitySize; \
                                    table->size = 0;                \
                                    table->deletedNum = 0;          \
                                    table->loadFactor = 0;          \
                                    table->dirtyFactor = 0;         \
                                } while(0)

#define CALC_FACTORS    do{                                                                     \
                            table->loadFactor = ((float)table->size) / ((float)table->capacity);\
                            table->dirtyFactor = table->deletedNum / table->capacity;           \
                        } while(0)

#ifndef  TEST
void* my_calloc(size_t nmem, size_t size){

    return calloc(nmem, size);
}
#else 
extern int throw_null;
void* my_calloc(size_t nmem, size_t size){

    if (throw_null == 1){
        
        return NULL;
    }

    if (throw_null > 1){

        throw_null--;
    }

    return calloc(nmem, size);
}
#endif 



int check_table(HashTable* table){

    if (table == NULL){

        fprintf(stderr, "Warning: check_table: table: NULL\n");
        return 0;
    }

    if (table->capacity <= 0){

        fprintf(stderr, "Error: check_table: table->capacity: table data corrupted\n");
        return 0;
    }

    if (table->size < 0){

        fprintf(stderr, "Error: check_table: table->size: table data corrupted {%i}\n", table->size);
        return 0;
    }

    if (table->deletedNum < 0){

        fprintf(stderr, "Error: check_table: table->deletedNum: table data corrupted {%i}\n", table->size);
        return 0;
    }

    if (table->loadFactor < 0 || table->loadFactor > 1){

        fprintf(stderr, "Error: check_table: table->loadFactor: table data corrupted\n");
        return 0;
    }

    if (table->dirtyFactor < 0 || table->dirtyFactor > 1){

        fprintf(stderr, "Error: check_table: table->dirtyFactor: table data corrupted\n");
        return 0;
    }

    return 1;
}

unsigned Hash(const char* element){

    if (element == NULL){

        return 0;
    }

    int len = strlen(element);
    unsigned long long ret_val = 0;
    unsigned long long factor = 11;

    for (int i = 0; i < len; i++){

        ret_val += element[i];
        factor *= 11;
    }  

    return ret_val;
}

int cmpKeys(TableCell* curCell, const char* key){

    if (strcmp(curCell->key, key) == 0){

        return 1;
    } else{

        return 0;
    }
}

HashTable* createHashTable(int capacity){

    HashTable* table = (HashTable*) my_calloc(1, sizeof(HashTable));

    if (table == NULL){

        fprintf(stderr, "Error: createHashTable(): ht: bad_alloc\n");
        return NULL;
    }
    
    if (capacity <= MIN_CAPACITY){

        capacity = MIN_CAPACITY;
    }

    table->content = (TableCell*) my_calloc(capacity, sizeof(TableCell));

    if (table->content == NULL){

        fprintf(stderr, "Error: createHashTable(): ht->content: bad_alloc\n");
        free(table);
        return NULL;
    }

    INIT_TABLE(capacity);

    return table;
}

void insertCell_(HashTable* table, TableCell* cell){

    if (table == NULL || cell == NULL){

        return;
    }

    Key key = cell->key;
    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;

    FIND_ELEM(return);

    *curCell = *cell;
    assert(curCell->valid == 1);
    table->size++;
    CALC_FACTORS;
}

void rehash_(HashTable* table, int rehashFactor){ 

    if (table == NULL || rehashFactor <= 0){

        return;
    }

    TableCell* newContent = (TableCell*) my_calloc(table->capacity * rehashFactor, sizeof(TableCell));

    if (newContent == NULL){

        fprintf(stderr, "Warning: rehash: newContent: bad_alloc\n");
        return;
    }

    TableCell* oldContent = table->content;
    table->content = newContent;

    int oldCapacity = table->capacity;

    INIT_TABLE(table->capacity * rehashFactor);
    
    for (int i = 0; i < oldCapacity; i++){

        if (oldContent[i].valid == 1){

            insertCell_(table, oldContent + i);
        }
    }
    assert(table->loadFactor < 0.9);
    free(oldContent);
}

void rehash(HashTable* table){

    if (check_table(table) == 0){

        return;
    }

    if (table->loadFactor > MAX_LOADFACTOR){ 
        
        rehash_(table, REHASH_FACTOR);
        return;
    }

    if (table->dirtyFactor > MAX_DIRTYFACTOR){

        rehash_(table, 1);
        return;
    }
}

void freeHashTable(HashTable* table){

    if (check_table(table) == 0){
        
        fprintf(stderr, "Can't free table\n");
        return;
    }

    for (int i = 0; i < table->capacity; i++){

        if (table->content[i].valid == 1){

            free(table->content[i].key);
            free(table->content[i].element);
        }
    }

    free(table->content);
    free(table);
}

Key copyKey_(Key key){
    
    int len = strlen(key) + 1;
    Key ret = (Key) my_calloc(len, sizeof(char));

    if (ret != NULL){

        memcpy(ret, key, len);
    }

    return ret;
}

int addNewElem_(HashTable* table, Key key, void* element, int elementLen){

    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;

    while (curCell->valid == 1){

        curCell = table->content + (initialPos + shift) % table->capacity;
        shift++;
    }

    curCell->element = (char*) my_calloc(elementLen, sizeof(char));

    if (curCell->element == NULL){

        fprintf(stderr, "Error: add_elem: table[elem]: bad_alloc\n");
        return -1;
    }
    
    if (memcpy(curCell->element, element, elementLen) == NULL){

        fprintf(stderr, "Error: addElem: memcpy: line was not copied\n");
        free(curCell->element);
        return -1;
    }   

    curCell->key = copyKey_(key);                                            
    
    if (curCell->key == NULL){

        fprintf(stderr, "Error: addElem: memcpy: key was not copied\n");
        free(curCell->element);
        return -1;
    } 

    if (curCell->valid == -1){

        table->deletedNum--;
    }

    curCell->elementSize = elementLen;
    curCell->valid = 1; 

    table->size++;
    CALC_FACTORS;

    return 1;
}

int addElem(HashTable* table, Key key, void* element, int elementLen){
    
    if (check_table(table) == 0){

        return -1;
    }

    if (key == NULL){
        
        fprintf(stderr, "Warning: addElem: key: NULL\n");
        return -1;
    }
    
    rehash(table);
    assert(table->loadFactor < 0.9);

    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;
    
    FIND_ELEM(return -1);

    if (curCell->valid == 1 && cmpKeys(curCell, key) == 1){

        return 0;
    }
    
    return addNewElem_(table, key, element, elementLen);
}

void* copyElem_(TableCell* cell){
    
    int len = cell->elementSize;
    Key ret = (Key) calloc(len, sizeof(char));

    if (ret != NULL){

        memcpy(ret, cell->element, len);
    }

    return ret;
}

void* getElem(HashTable* table, Key key){

    if (check_table(table) == 0){

        return NULL;
    }

    if (key == NULL){

        fprintf(stderr, "Warning: getElem: element: NULL\n");
        return NULL;
    }
    
    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;

    FIND_ELEM(return NULL);

    if (curCell->valid == 1 && cmpKeys(curCell, key) == 1){

        return copyElem_(curCell);
    } else{

        return NULL;
    }
}

void removeElem(HashTable* table, Key key){

    if (check_table(table) == 0){

        return;
    }

    if (key == NULL){

        fprintf(stderr, "Warning: removeElem: element: NULL\n");
        return;
    }

    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;

    FIND_ELEM(return);

    if (curCell->valid == 1 && cmpKeys(curCell, key) == 1){

        curCell->valid = -1;
        free(curCell->element);
        free(curCell->key);
        curCell->key = NULL;
        curCell->element = NULL;

        table->size--;
        table->deletedNum++;
        CALC_FACTORS;

        rehash(table);
    }
}
