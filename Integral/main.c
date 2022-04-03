#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include <sched.h>

#include "Integral.h"

#define PAGE_SIZE 4076
#define NUM_OF_LOG_CPU 12
#define MAGIC

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)       

struct thread_data{

    double from;
    double to;
    int thread_num;
    double result[PAGE_SIZE / sizeof(double)];
};

void* thread_body(void* args){

    struct thread_data* data = (struct thread_data*) args;
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(data->thread_num, &cpuset);
    
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    
    sched_yield();
    
    *data->result = definite_integral(data->from, data->to);

    return NULL;
}

void magic(int num_of_threads, pthread_t* thr_info){

    struct thread_data* data = (struct thread_data*) calloc(num_of_threads, sizeof(struct thread_data));
    if (data == NULL) CHECK_ERROR("alloc data:");

    for (int i = 0; i < num_of_threads; i++){

        data[i].from = i * (RANGE / num_of_threads);
        data[i].to = data[i].from + (RANGE / num_of_threads);
        data[i].thread_num = i;

        if (pthread_create(thr_info + i, NULL, thread_body, data + i) != 0) CHECK_ERROR("thread create:");
    }
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

    pthread_t* thr_info = (pthread_t*) calloc((num_of_threads > NUM_OF_LOG_CPU) ? num_of_threads : NUM_OF_LOG_CPU, sizeof(pthread_t));
    if (thr_info == NULL) CHECK_ERROR("alloc threads:");

    struct thread_data* data = (struct thread_data*) calloc(num_of_threads, sizeof(struct thread_data));
    if (data == NULL) CHECK_ERROR("alloc data:");

    #ifdef MAGIC
    magic(num_of_threads, thr_info);
    #endif

    for (int i = 0; i < num_of_threads; i++){

        data[i].from = i * (RANGE / num_of_threads);
        data[i].to = data[i].from + (RANGE / num_of_threads);
        data[i].thread_num = i;

        if (pthread_create(thr_info + i, NULL, thread_body, data + i) != 0) CHECK_ERROR("thread create:");
    }

    double integral = 0;

    for (int i = 0; i < num_of_threads; i++){

        if (pthread_join(thr_info[i], NULL) != 0) CHECK_ERROR("thread join:");
        
        integral += data[i].result[0];
    }

    free(thr_info);
    free(data);
}
