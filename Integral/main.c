#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Integral.h"

#define PAGE_SIZE 4096
#define FROM 1
#define TO   100


int main(int argc, char** argv){

    if (argc < 2){
        
        printf("Not enough parameters\n");
        return 0;
    }

    int num_of_threads = atoi(argv[1]);

    if (num_of_threads < 1){

        printf("Not enough threads\n");
        return 0;
    }

    double res = definite_integral(FROM, TO);

    printf("Result = %lf\n", res);
}
