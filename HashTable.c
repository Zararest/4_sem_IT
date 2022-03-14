#include "HashTable.h"
    
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

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

#ifndef  TEST
void* my_calloc(size_t nmem, size_t size){

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

    if (table->loadFactor < 0 || table->loadFactor > 1){

        fprintf(stderr, "Error: check_table: table->loadFactor: table data corrupted\n");
        return 0;
    }

    return 1;
}

Key Hash(const char* element){

    if (element == NULL){

        return 0;
    }

    int len = strlen(element);
    unsigned long long ret_val = 0;
    unsigned long long factor = 11;

    for (int i = 0; i < len; i++){

        ret_val += element[i] * factor;
        factor *= 11;
    }  

    return ret_val;
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
    ht->capacity = capacity;
    ht->loadFactor = 0;

    return ht;
}

void rehash(HashTable* table){

    if (check_table(table) == 0){

        return;
    }

    if (table->loadFactor < 0.5){ 

        return;
    }

    TableCell* newContent = (TableCell*) my_calloc(table->capacity * REHASH_FACTOR, sizeof(TableCell));

    if (newContent == NULL){

        fprintf(stderr, "Warning: rehash: newContent: bad_alloc\n");
        return;
    }

    TableCell* oldContent = table->content;
    
    table->content = newContent;
    
    for (int i = 0; i < table->capacity; i++){

        if (oldContent[i].valid == 1){

            Key newPos = oldContent[i].key % (table->capacity * REHASH_FACTOR);

            newContent[newPos].element = oldContent[i].element;
            newContent[newPos].key = oldContent[i].key;
            newContent[newPos].valid = 1;
        }
    }

    table->capacity = table->capacity * REHASH_FACTOR;
    table->loadFactor = table->size / table->capacity;

    free(oldContent);
}

void freeHashTable(HashTable* table){

    if (check_table(table) == 0){
        
        fprintf(stderr, "Can't free table\n");
        return;
    }

    for (int i = 0; i < table->capacity; i++){

        if (table->content[i].valid == 1){

            free(table->content[i].element);
        }
    }

    free(table->content);
    free(table);
}

Key addElem(HashTable* table, const char* element){
    
    if (check_table(table) == 0){

        return -1;
    }

    if (element == NULL){
        
        fprintf(stderr, "Warning: addElem: element: NULL\n");
        return -1;
    }
    
    rehash(table);
    
    Key key = Hash(element);
    Key curPos = key % table->capacity;
    int elems_len = strlen(element);
    
    while (table->content[curPos].valid == 1){

        curPos = (curPos + 1) % table->capacity;

        if (curPos == key % table->capacity){
            
            fprintf(stderr, "Error: addElem: table corrupted\n");
            return -1;
        }
    }
    
    table->content[curPos].element = (char*) my_calloc(elems_len, sizeof(char));

    if (table->content[curPos].element == NULL){

        fprintf(stderr, "Error: add_elem: table[elem]: bad_alloc\n");
        return -1;
    }
    
    if (memcpy(table->content[curPos].element, element, elems_len) == NULL){

        fprintf(stderr, "Error: addElem: memcpy: line was not copied\n");
        return -1;
    }   

    table->content[curPos].key = key;                                            
    table->content[curPos].valid = 1; 

    table->size++;
    table->loadFactor = table->size / table->capacity;

    return key;
}

char* getElem(HashTable* table, Key key){

    if (check_table(table) == 0){

        return NULL;
    }
    
    Key curPos = key % table->capacity;
    TableCell* curCell = table->content + curPos;

    while (curCell->valid == 1 && curCell->key != key){

        curPos = (curPos + 1) % table->capacity;
        curCell = table->content + curPos;

        if (curPos == key % table->capacity){
            
            fprintf(stderr, "Error: getElem: table corrupted\n");
            return NULL;
        }
    }

    if (curCell->valid == 1 && curCell->key == key){

        return curCell->element;
    } else{

        return NULL;
    }
}

Bool cmpElems(TableCell* curCell, const char* element){

    if (strcmp(curCell->element, element) == 0){

        return 1;
    } else{

        return 0;
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

    Key key = Hash(element);
    Key curPos = key % table->capacity;
    TableCell* curCell = table->content + curPos;

    while (curCell->valid == 1){

        if (cmpElems(curCell, element) == 1){

            curCell->valid = 0;
            free(curCell->element);

            table->size--;
            table->loadFactor = table->size / table->capacity;

            return;
        }

        curPos = (curPos + 1) % table->capacity;
        curCell = table->content + curPos;

        if (curPos == key % table->capacity){
            
            fprintf(stderr, "Error: removeElem: table corrupted\n");
            return;
        }
    }
}