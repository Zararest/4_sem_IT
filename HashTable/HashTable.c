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

    HashTable* ht = (HashTable*) my_calloc(1, sizeof(HashTable));

    if (ht == NULL){

        fprintf(stderr, "Error: createHashTable(): ht: bad_alloc\n");
        return NULL;
    }
    
    if (capacity <= MIN_CAPACITY){

        capacity = MIN_CAPACITY;
    }

    ht->content = (TableCell*) my_calloc(capacity, sizeof(TableCell));

    if (ht->content == NULL){

        fprintf(stderr, "Error: createHashTable(): ht->content: bad_alloc\n");
        free(ht);
        return NULL;
    }

    ht->size = 0;
    ht->deletedNum = 0;
    ht->capacity = capacity;
    ht->loadFactor = 0;
    ht->dirtyFactor = 0;

    return ht;
}

void insertCell_(HashTable* table, TableCell* cell){

    if (table == NULL || cell == NULL){

        return;
    }

    unsigned hash = Hash(cell->key);
    unsigned curPos = hash % table->capacity;
    
    while (table->content[curPos].valid == 1){

        if (cmpKeys(table->content + curPos, cell->key) == 1){

            fprintf(stderr, "Error: insertCell: same keys\n");
            return;
        }

        curPos = (curPos + 1) % table->capacity;

        if (curPos == hash % table->capacity){
            
            fprintf(stderr, "Error: insertCell: table corrupted\n");
            return;
        }
    }

    table->content[curPos] = *cell;
    assert(table->content[curPos].valid == 1);
    table->size++;

    table->loadFactor = ((float)table->size) / ((float)table->capacity);
    assert(table->loadFactor < 0.9);
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
    table->capacity = table->capacity * rehashFactor;
    table->size = 0;
    table->deletedNum = 0;
    table->loadFactor = 0;
    table->dirtyFactor = 0;
    
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

    unsigned hash = Hash(element);
    unsigned curPos = hash % table->capacity;
    
    while (table->content[curPos].valid != 0){

        if (table->content[curPos].valid == 1 && cmpKeys(table->content + curPos, key) == 1){

            return 0;
        }

        curPos = (curPos + 1) % table->capacity;

        if (curPos == hash % table->capacity){
            
            fprintf(stderr, "Error: addElem: table corrupted\n");
            return -1;
        }
    }
    
    table->content[curPos].element = (char*) my_calloc(elementLen, sizeof(char));

    if (table->content[curPos].element == NULL){

        fprintf(stderr, "Error: add_elem: table[elem]: bad_alloc\n");
        return -1;
    }
    
    if (memcpy(table->content[curPos].element, element, elementLen) == NULL){

        fprintf(stderr, "Error: addElem: memcpy: line was not copied\n");
        free(table->content[curPos].element);
        return -1;
    }   

    table->content[curPos].key = copyKey_(key);                                            
    
    if (table->content[curPos].key == NULL){

        fprintf(stderr, "Error: addElem: memcpy: key was not copied\n");
        free(table->content[curPos].element);
        return -1;
    } 

    if (table->content[curPos].valid == -1){

        table->deletedNum--;
    }

    table->content[curPos].elementSize = elementLen;
    table->content[curPos].valid = 1; 

    table->size++;
    table->loadFactor = ((float)table->size) / ((float)table->capacity);
    assert(table->loadFactor < 0.9);
    table->dirtyFactor = table->deletedNum / table->capacity;

    return 1;
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
    
    unsigned curPos = Hash(key) % table->capacity, initialPos = curPos;
    TableCell* curCell = table->content + curPos;

    while (curCell->valid == -1 || (curCell->valid == 1 && cmpKeys(curCell, key) == 0)){

        curPos = (curPos + 1) % table->capacity;
        curCell = table->content + curPos;

        if (curPos == initialPos){
            
            fprintf(stderr, "Error: getElem: table corrupted ");
            return NULL;
        }
    }

    if (curCell->valid == 1 && cmpKeys(curCell, key) == 1){

        return copyElem_(curCell);
    } else{

        return NULL;
    }
}

void removeElem(HashTable* table, const char* element){

    if (check_table(table) == 0){

        return;
    }

    if (element == NULL){

        fprintf(stderr, "Warning: removeElem: element: NULL\n");
        return;
    }

    rehash(table);

    unsigned hash = Hash(element);
    unsigned curPos = hash % table->capacity;
    TableCell* curCell = table->content + curPos;

    while (curCell->valid != 0){

        if (curCell->valid == 1 && cmpKeys(curCell, element) == 1){

            curCell->valid = -1;
            free(curCell->element);
            free(curCell->key);
            curCell->key = NULL;
            curCell->element = NULL;

            table->size--;
            table->deletedNum++;
            table->loadFactor = ((float)table->size) / ((float)table->capacity);
            assert(table->loadFactor < 0.9);
            table->dirtyFactor = table->deletedNum / table->capacity;

            return;
        }

        curPos = (curPos + 1) % table->capacity;
        curCell = table->content + curPos;

        if (curPos == hash % table->capacity){
            
            fprintf(stderr, "Error: removeElem: table corrupted\n");
            return;
        }
    }
}