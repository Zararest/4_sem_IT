#include "TestTable.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int throw_null;

#ifdef TEST
void* my_calloc(size_t nmem, size_t size){

    if (throw_null == 1){

        return NULL;
    }

    return calloc(nmem, size);
}
#endif

int test_createTable(){

    throw_null = 1;    
    HashTable* table_0 = createHashTable(11);
    int num_of_errors = table_0 != NULL;
    
    throw_null = 0;

    HashTable* table_1 = createHashTable(10);

    if (table_1 != NULL){

        num_of_errors += table_1->size != 0;
        num_of_errors += table_1->capacity != 10;
        num_of_errors += table_1->loadFactor != 0;

        for (int i = 0; i < 10; i++){

            num_of_errors += table_1->content[i].element != NULL;    
        }
    }

    HashTable* table_2 = createHashTable(-10);
    
    if (table_2 != NULL){

        num_of_errors += table_2->capacity != MIN_CAPACITY;
    }

    freeHashTable(table_1);
    freeHashTable(table_2);

    return num_of_errors; 
}

int test_rehash(HashTable* table){

    throw_null = 0;

    HashTable* table_1 = createHashTable(20);
    char lines_arr[10][25] ={"aaaaaaaaaaa",
                            "bbbbbbbbbbbbb",
                            "cccccccccccc",
                            "ddddddddddd",
                            "eeeeeeeeeeeeeee",
                            "fffffffffffffffff",
                            "ggggggggggggggggggg",
                            "hhhhhhh",
                            "iiiiiiiiii",
                            "j"};
    
    Key keys_arr[10] = {0};
    int num_of_errors = 0;
    
    for (int i = 0; i < 10; i++){
        
        keys_arr[i] = addElem(table_1, lines_arr[i]);
    }

    throw_null = 1;
    rehash(table_1);
    throw_null = 0;
    rehash(table_1);

    for (int i = 0; i < 10; i++){

        if (getElem(table_1, keys_arr[i]) != NULL){

            num_of_errors += strcmp(getElem(table_1, keys_arr[i]), lines_arr[i]);
        }
    }

    HashTable* table_2 = createHashTable(20);
    char lines_arr_2[10][25]={"aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "ddddddddddd",
                            "eeeeeeeeeeeeeee",
                            "fffffffffffffffff",
                            "aaaaaaaaaaa",
                            "hhhhhhh",
                            "iiiiiiiiii",
                            "j"};
    
    for (int i = 0; i < 10; i++){

        keys_arr[i] = addElem(table_2, lines_arr_2[i]);
    }

    rehash(table_2);

    for (int i = 0; i < 10; i++){

        if (getElem(table_2, keys_arr[i]) != NULL){

            num_of_errors += strcmp(getElem(table_2, keys_arr[i]), lines_arr_2[i]);
        }
    }

    HashTable* table_3 = createHashTable(100);
    table_3->capacity = -1;
    rehash(table_3);
    table_3->capacity = 100;

    freeHashTable(table_1);
    freeHashTable(table_2);
    freeHashTable(table_3);

    return num_of_errors;
}

int test_addElem(){

    throw_null = 0;

    HashTable* table = createHashTable(10);
    char line[11] = {"abcdefghi\0"};
    int num_of_errors = 0;

    for (int i = 0; i < 10; i++){

        table->content[i].valid = 1;
        table->content[i].element = line;
    }

    num_of_errors += addElem(table, line) != -1;
    free(table->content);
    free(table);

    HashTable* table_2 = createHashTable(10);
    char lines_arr[10][25] ={"aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "ddddddddddd",
                            "eeeeeeeeeeeeeee",
                            "fffffffffffffffff",
                            "aaaaaaaaaaa",
                            "hhhhhhh",
                            "iiiiiiiiii",
                            "j"};
    
    throw_null = 1;

    for (int i = 0; i < 10; i++){
        
        addElem(table_2, lines_arr[i]);
    }

    num_of_errors += addElem(table_2, NULL) != -1;
    num_of_errors += addElem(NULL, NULL) != -1;

    freeHashTable(table_2);

    return num_of_errors;
}

int test_getElem(){

    throw_null = 0;

    HashTable* table_1 = createHashTable(10);
    char lines_arr[10][25] ={"aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "ddddddddddd",
                            "eeeeeeeeeeeeeee",
                            "fffffffffffffffff",
                            "aaaaaaaaaaa",
                            "hhhhhhh",
                            "iiiiiiiiii",
                            "j"};
    Key keys_arr[10] = {0};
    int num_of_errors = 0;

    for (int i = 0; i < 10; i++){

        keys_arr[i] = addElem(table_1, lines_arr[i]);
    }

    for (int i = 0; i < 10; i++){

        if (getElem(table_1, keys_arr[i]) != NULL){
            
            num_of_errors += strcmp(getElem(table_1, keys_arr[i]), lines_arr[i]);
        }
    }

    num_of_errors += getElem(table_1, 0) != NULL;

    HashTable* table_2 = createHashTable(10);
    char tmp_line[2] = {'\0'}; 

    for (int i = 0; i < 10; i++){

        table_2->content[i].valid = 1;
        table_2->content[i].element = tmp_line;
    }

    num_of_errors += getElem(table_2, 1) != NULL;
    num_of_errors += getElem(NULL, 0) != NULL;

    free(table_2->content);
    free(table_2);
    freeHashTable(table_1);

    return num_of_errors;
}

int test_removeElem(){

    throw_null = 0;

    HashTable* table_1 = createHashTable(10);
    char lines_arr[10][25] ={"aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "aaaaaaaaaaa",
                            "ddddddddddd",
                            "eeeeeeeeeeeeeee",
                            "fffffffffffffffff",
                            "aaaaaaaaaaa",
                            "hhhhhhh",
                            "iiiiiiiiii",
                            "j"};
    Key keys_arr[10] = {0};
    int num_of_errors = 0;

    for (int i = 0; i < 10; i++){

        keys_arr[i] = addElem(table_1, lines_arr[i]);
    }

    removeElem(table_1, lines_arr[0]);

    num_of_errors += getElem(table_1, keys_arr[0]) != NULL;

    HashTable* table_2 = createHashTable(10);
    char tmp_line[2] = {'\0'};

    for (int i = 0; i < 10; i++){

        table_2->content[i].valid = 1;
        table_2->content[i].element = tmp_line;
    }

    removeElem(table_2, "aaa");
    removeElem(NULL, 0);
    removeElem(table_1, NULL);

    free(table_2->content);
    free(table_2);
    freeHashTable(table_1);

    return num_of_errors;
}

int tets_negatives(){

    int num_of_errors = 0;

    num_of_errors += Hash(NULL) != 0;
    
    HashTable* table = createHashTable(10);
    HashTable* table_1 = NULL;

    table->loadFactor = 2;
    num_of_errors += addElem(table, "a") != -1;
    table->loadFactor = 0;

    table->size = -1;
    num_of_errors += addElem(table, "a") != -1;
    
    free(table->content);
    free(table);
    
    throw_null = 1;

    table_1 = createHashTable(10);
    num_of_errors += table_1 != NULL;

    throw_null = 0;

    freeHashTable(NULL);

    HashTable* table_2 = createHashTable(10);
    void* old_ptr = table_2->content;

    rehash(table_2);
    num_of_errors += old_ptr != table_2->content;

    throw_null = 0;

    freeHashTable(table_2);

    return num_of_errors;
}