#include "TestTable.h"
#include <stdio.h>

int main(){

    printf("Num of errors createTable: %i\n", test_createTable());
    printf("Num of errors rehash: %i\n", test_rehash());
    printf("Num of errors addElem: %i\n", test_addElem());
    printf("Num of errors get_elem: %i\n", test_getElem());
    printf("Num of errors removeElem: %i\n", test_removeElem());
    printf("Num of errors negatives: %i\n", tets_negatives());
}