#include "HashTable.h"

struct test{

    int a;
    float b;
};

int main(){

    HashTable* ht = createHashTable(10);
    char tmp[20] = {[1] = 0};
    struct test test_ = {.b = 1};
}