#include "TestTable.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int throw_null;

#ifdef TEST
void* my_calloc(size_t nmem, sizt_t size){

    if (rand() % 3 == 0 && throw_null == 1){

        return NULL;
    }

    return calloc(nmem, size);
}
#endif

int test_createTable(){

    throw_null = 1;

    HashTable* table_1 = createHashTable(10);
    int num_of_errors = 0;

    if (table_1 != NULL){

        num_of_errors += table_1->size != 0;
        num_of_errors += table_1->capacity != 10;
        num_of_errors += table_1->loadFactor != 0;

        for (int i = 0; i < 10; i++){

            num_of_errors += table_1->content[i].element != NULL;    
        }
    }

    throw_null = 0;

    HashTable* table_2 = createHashTable(-10);
    
    if (table_2 != NULL){

        num_of_errors += table_2->capacity != MIN_CAPACITY;
    }

    freeHashTable(table_1);
    freeHashTable(table_2);

    if (num_of_errors != 0){

        fprintf("In test_createTable() number of error = %i\n", num_of_errors);
        return 0;
    } else{

        return 1;
    }    
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

    for (int i = 0; i < 10; i++){

        if (get_elem(table_1, keys_arr[i]) != NULL){

            num_of_errors += strcmp(get_elem(table_1, keys_arr[i]), lines_arr[i]);
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

    throw_null = 1;
    rehash(table_2);
    throw_null = 0;

    for (int i = 0; i < 10; i++){

        if (get_elem(table_2, keys_arr[i]) != NULL){

            num_of_errors += strcmp(get_elem(table_2, keys_arr[i]), lines_arr_2[i]);
        }
    }

    freeHashTable(table_1);
    freeHashTable(table_2);

    if (num_of_errors != 0){

        fprintf("In test_rehash() number of error = %i\n", num_of_errors);
        return 0;
    } else{

        return 1;
    }  
}

int test_addElem(){

    throw_null = 0;

    HashTable* table = createHashTable(10);
    char line[11] = {"abcdefghi\0"};
    int num_of_errors = 0;

    for (int i = 0; i < 10; i++){

        table->content[i].element = line;
    }

    num_of_errors += addElem(table, line) != -1;

    throw_null = 1;

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
    
    for (int i = 0; i < 10; i++){
        
        addElem(table_2, lines_arr[i]);
    }

}
