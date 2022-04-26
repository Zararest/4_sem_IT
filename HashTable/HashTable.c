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

int cmpKeys_(TableCell* curCell, const char* key){

    if (strcmp(curCell->key, key) == 0){

        return 1;
    } else{

        return 0;
    }
}

void calcFactors_(HashTable* table){

    table->loadFactor = ((float)table->size) / ((float)table->capacity);
    table->dirtyFactor = table->deletedNum / table->capacity; 
}

void initTable_(HashTable* table, int capacitySize){

    table->capacity = (capacitySize);
    table->size = 0;                
    table->deletedNum = 0;          
    table->loadFactor = 0;         
    table->dirtyFactor = 0;  
}

TableCell* findElem_(HashTable* table, Key key, int* err){

    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;

    while (curCell->valid == -1 || (curCell->valid == 1 && cmpKeys_(curCell, key) == 0)){
                                                                                    
        curCell = table->content + (initialPos + shift) % table->capacity;              
        shift++;                                                                        
                                                                                        
        if (shift == table->capacity){                                                  
                       
            *err = LoopedSearch;
            return NULL;                                                          
        }                                                                               
    } 

    return curCell;
}

void what(int err){

    if (err == Ok) return;

    switch (err){

    case BadAlloc:
        fprintf(stderr, "Error: bad alloc\n");
        break;

    case MemcpyException:
        fprintf(stderr, "Error: memory was not copied\n");
        break;

    case CapacityCorrupted:
        fprintf(stderr, "Error: table data corrupted: table->capacity\n");
        break;

    case SizeCorrupted:
        fprintf(stderr, "Error: table data corrupted: table->size\n");
        break;

    case DeletedNumCorrupted:
        fprintf(stderr, "Error: table data corrupted: table->deletedNum\n");
        break;

    case LoadFactorCorrupted:
        fprintf(stderr, "Error: table data corrupted: table->loadFactor\n");
        break;

    case DirtyFactorCorrupted:
        fprintf(stderr, "Error: table data corrupted: table->dirtyFactor\n");
        break;

    case LoopedSearch:
        fprintf(stderr, "Error: table data corrupted: looped element search\n");
        break;

    case WrongParam:
        fprintf(stderr, "Error: incorrect function parameters\n");
        break;

    default:
        fprintf(stderr, "Undefined error\n");
    }
}

int check_table_(HashTable* table, int* err){

    if (table == NULL){

        *err = WrongParam;
        return 0;
    }

    if (table->capacity <= 0){

        *err = CapacityCorrupted;
        return 0;
    }

    if (table->size < 0){

        *err = SizeCorrupted;
        return 0;
    }

    if (table->deletedNum < 0){

        *err = DeletedNumCorrupted;
        return 0;
    }

    if (table->loadFactor < 0 || table->loadFactor > 1){

        *err = LoadFactorCorrupted;
        return 0;
    }

    if (table->dirtyFactor < 0 || table->dirtyFactor > 1){

        *err = DirtyFactorCorrupted;
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

HashTable* createHashTable(int capacity, int* err){

    HashTable* table = (HashTable*) my_calloc(1, sizeof(HashTable));

    if (table == NULL){

        *err = BadAlloc;
        return NULL;
    }
    
    if (capacity <= MIN_CAPACITY){

        capacity = MIN_CAPACITY;
    }

    table->content = (TableCell*) my_calloc(capacity, sizeof(TableCell));

    if (table->content == NULL){

        *err = BadAlloc;
        free(table);
        return NULL;
    }

    initTable_(table, capacity);

    return table;
}

void insertCell_(HashTable* table, TableCell* cell, int* err){ //сделать все подчеркнутые статиком 

    Key key = cell->key;
    TableCell* curCell = findElem_(table, key, err);

    if (curCell == NULL){

        return;
    }

    *curCell = *cell;
    table->size++;
    calcFactors_(table);
}

void rehash_(HashTable* table, int rehashFactor, int* err){ 

    TableCell* newContent = (TableCell*) my_calloc(table->capacity * rehashFactor, sizeof(TableCell));

    if (newContent == NULL){

        *err = BadAlloc;
        return;
    }

    TableCell* oldContent = table->content;
    table->content = newContent;

    int oldCapacity = table->capacity;

    initTable_(table, table->capacity * rehashFactor);
    
    for (int i = 0; i < oldCapacity; i++){

        if (oldContent[i].valid == 1){

            insertCell_(table, oldContent + i, err);
        }
    }
    
    free(oldContent);
}

void rehash(HashTable* table, int* err){

    if (check_table_(table, err) == 0){

        return;
    }

    if (table->loadFactor > MAX_LOADFACTOR){ 
        
        rehash_(table, REHASH_FACTOR, err);
        return;
    }

    if (table->dirtyFactor > MAX_DIRTYFACTOR){

        rehash_(table, 1, err);
        return;
    }
}

void freeHashTable(HashTable* table, int* err){

    if (check_table_(table, err) == 0){
        
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

int addNewElem_(HashTable* table, Key key, void* element, int elementLen, int* err){

    unsigned initialPos = Hash(key) % table->capacity;
    unsigned shift = 0;
    TableCell* curCell = table->content + initialPos;

    while (curCell->valid == 1){

        curCell = table->content + (initialPos + shift) % table->capacity;
        shift++;
    }

    curCell->element = (char*) my_calloc(elementLen, sizeof(char));

    if (curCell->element == NULL){

        *err = BadAlloc;
        return -1;
    }
    
    if (memcpy(curCell->element, element, elementLen) == NULL){

        *err = MemcpyException;
        free(curCell->element);
        return -1;
    }   

    curCell->key = copyKey_(key);                                            
    
    if (curCell->key == NULL){

        *err = MemcpyException;
        free(curCell->element);
        return -1;
    } 

    if (curCell->valid == -1){

        table->deletedNum--;
    }

    curCell->elementSize = elementLen;
    curCell->valid = 1; 

    table->size++;
    calcFactors_(table);

    return 1;
}

int addElem(HashTable* table, Key key, void* element, int elementLen, int* err){
    
    if (check_table_(table, err) == 0){

        return -1;
    }

    if (key == NULL){
        
        *err = WrongParam;
        return -1;
    }

    *err = Ok;
    rehash(table, err);

    if (*err != Ok){

        return -1;
    }

    TableCell* curCell = findElem_(table, key, err);
    
    if (curCell == NULL){

        return -1;
    }

    if (curCell->valid == 1){

        return 0;
    }
    
    return addNewElem_(table, key, element, elementLen, err);
}

void* copyElem_(TableCell* cell){
    
    int len = cell->elementSize;
    Key ret = (Key) calloc(len, sizeof(char));

    if (ret != NULL){

        memcpy(ret, cell->element, len);
    }

    return ret;
}

void* getElem(HashTable* table, Key key, int* err){

    if (check_table_(table, err) == 0){

        return NULL;
    }

    if (key == NULL){

        *err = WrongParam;
        return NULL;
    }
    
    TableCell* curCell = findElem_(table, key, err);

    if (curCell == NULL){

        return NULL;
    }

    if (curCell->valid == 1){

        return copyElem_(curCell);
    } else{

        return NULL;
    }
}

void removeElem(HashTable* table, Key key, int* err){

    if (check_table_(table, err) == 0){

        return;
    }

    if (key == NULL){

        *err = WrongParam;
        return;
    }

    TableCell* curCell = findElem_(table, key, err);

    if (curCell == NULL){
        
        return;
    }

    if (curCell->valid == 1){

        curCell->valid = -1;
        free(curCell->element);
        free(curCell->key);
        curCell->key = NULL;
        curCell->element = NULL;

        table->size--;
        table->deletedNum++;
        calcFactors_(table);

        rehash(table, err);
    }
}
