#include "HashTable.h"
    
#include <stdlib.h>
#include <string.h>

Key Hash(const char* element){

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

    HashTable* ht = (HashTable*) calloc(1, sizeof(HashTable));
    
    if (capacity <= MIN_CAPACITY){

        capacity = MIN_CAPACITY;
    }

    ht->content = (TableCell*) calloc(capacity, sizeof(TableCell));
    ht->size = 0;
    ht->capacity = capacity;
    ht->loadFactor = 0;

    return ht;
}

void rehash(HashTable* table){

    if (table->loadFactor < 0.5){ 

        return;
    }

    TableCell* newContent = (TableCell*) calloc(table->capacity * REHASH_FACTOR, sizeof(TableCell));
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

    for (int i = 0; i < table->capacity; i++){

        if (table->content[i].valid == 1){

            free(table->content[i].element);
        }
    }

    free(table->content);
    free(table);
}

int len(const char* line){

    return strlen(line);
}

Key addElem(HashTable* table, const char* element){

    rehash(table);

    Key key = Hash(element);
    Key curPos = key % table->capacity;

    while (table->content[curPos].valid == 1){

        curPos = (curPos + 1) % table->capacity;

        CHECK_END_OF_TABLE;
    }

    PLACE_ELEMENT(curPos, element);

    table->size++;
    table->loadFactor = table->size / table->capacity;

    return key;
}

char* getElem(HashTable* table, Key key){

    Key curPos = key % table->capacity;
    TableCell* curCell = table->content + curPos;

    while (curCell->valid == 1 && curCell->key != key){

        curPos = (curPos + 1) % table->capacity;
        curCell = table->content + curPos;

        CHECK_END_OF_TABLE;
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

        CHECK_END_OF_TABLE;
    }
}