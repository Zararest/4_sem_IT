#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include <sched.h>

#include "Integral.h"

#define PAGE_SIZE 4096
#define NUM_OF_LOG_CPU 16

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)       

struct thread_data{

    double from;
    double to;
    int thread_num;
    double* result;
};

void* thread_body(void* args){

    struct thread_data* data = (struct thread_data*) args;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(data->thread_num, &cpuset);
    
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    
    //sched_yield();
    
    *data->result = definite_integral(data->from, data->to);

    //printf("thread num %i result: %lf [%lf, %lf]\n", data->thread_num, *data->result, data->from, data->to);
    return NULL;
}

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

    double* results = (double*) calloc(PAGE_SIZE * num_of_threads, 1);
    if (results == NULL) CHECK_ERROR("alloc results:");

    pthread_t* thr_info = (pthread_t*) calloc(num_of_threads, sizeof(int));
    if (thr_info == NULL) CHECK_ERROR("alloc threads:");

    struct thread_data* data = (struct thread_data*) calloc(num_of_threads, sizeof(struct thread_data));
    if (data == NULL) CHECK_ERROR("alloc data:");

    for (int i = 0; i < num_of_threads; i++){

        data[i].from = i * (RANGE / num_of_threads);
        data[i].to = data[i].from + (RANGE / num_of_threads);
        data[i].thread_num = (2 * i) % NUM_OF_LOG_CPU;
        data[i].result = results + i * (PAGE_SIZE / sizeof(double));

        if (pthread_create(thr_info + i, NULL, thread_body, data + i) != 0) CHECK_ERROR("thread create:");
    }

    double integral = 0;

    for (int i = 0; i < num_of_threads; i++){

        pthread_join(thr_info[i], NULL);
        integral += *(results + i * (PAGE_SIZE / sizeof(double)));
    }

    printf("Integral = %.2lf\n", integral);
    //printf("Time = %.4f delta = %lf\n", ((double)(end - start)) / CLOCKS_PER_SEC, DELTA_X);
}
