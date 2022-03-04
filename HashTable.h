#ifndef _HASHTABLE 
#define _HASHTABLE

#define MIN_CAPACITY 64
#define REHASH_FACTOR 2

#define CHECK_END_OF_TABLE  do{                                         \
                                if (curPos == key % table->capacity){   \
                                    exit(-1);                           \
                                }                                       \
                            } while(0)    

#define PLACE_ELEMENT(place, elem)   do{                                                                                \
                                            table->content[place].element = (char*) calloc(len(elem), sizeof(char));    \
                                            memcpy(table->content[place].element, elem, len(elem));                     \
                                            table->content[place].key = key;                                            \
                                            table->content[place].valid = 1;                                            \
                                        } while(0)                               

typedef int Key;
typedef int Bool;

typedef struct _TableCell{

    char* element;
    Key key;
    Bool valid;
} TableCell;

typedef struct _HashTable{

    TableCell* content;
    int size;
    int capacity;
    float loadFactor;
} HashTable;

HashTable* createHashTable(int capacity);
void rehash(HashTable* table);
void freeHashTable(HashTable* table);

Key addElem(HashTable* table, const char* element);
char* getElem(HashTable* table, Key key);
void removeElem(HashTable* table, const char* element);

Key Hash(const char* element);

#endif