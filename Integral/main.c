#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include <sched.h>
#include <sys/sysinfo.h>

#include "Integral.h"

#define PAGE_SIZE 4096
#define DATA_SIZE sizeof(double) * 2 + sizeof(int) 

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)       

struct thread_data{

    double from;
    double to;
    int thread_num;
    double result[(PAGE_SIZE - DATA_SIZE) / sizeof(double)];
};

int get_log_CPU_num(){

    return get_nprocs();
}

void* thread_body(void* args){

    struct thread_data* data = (struct thread_data*) args;
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(data->thread_num, &cpuset);
    
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    
    //sched_yield();
    
    *data->result = definite_integral(data->from, data->to);

    return NULL;
}


double get_result(int num_of_threads, pthread_t* thr_info, struct thread_data* data){

    double integral = 0;

    for (int i = 0; i < num_of_threads; i++){

        if (pthread_join(thr_info[i], NULL) != 0) CHECK_ERROR("thread join:");

        integral += data[i].result[0];
    }

    return integral;
}

void wait_for_magic(int num_of_threads, pthread_t* thr_info){

    for (int i = 0; i < num_of_threads; i++){

        if (pthread_join(thr_info[i], NULL) != 0) CHECK_ERROR("thread join:");
    }
}

int magic(int num_of_threads, pthread_t** thr_info, int num_of_real_threads){

    if (num_of_threads <= 0) return 0;

    double magic_koef = 1;
    *thr_info = (pthread_t*) calloc(num_of_threads, sizeof(pthread_t));
    if (thr_info == NULL) CHECK_ERROR("alloc threads:");

    struct thread_data* data = (struct thread_data*) calloc(num_of_threads, sizeof(struct thread_data));
    if (data == NULL) CHECK_ERROR("alloc data:");

    for (int i = 0; i < num_of_threads; i++){

        data[i].from = 0;
        data[i].to = (RANGE / num_of_real_threads) * magic_koef;
        data[i].thread_num = i;

        if (pthread_create((*thr_info) + i, NULL, thread_body, data + i) != 0) CHECK_ERROR("thread create:");
    }

    return num_of_threads;
}

void set_threads(int num_of_threads, pthread_t* thr_info, struct thread_data* data){

    pthread_t* magic_threads = NULL;
    int num_of_log_CPU = get_log_CPU_num();
    int num_of_magic_thr = magic(num_of_log_CPU - num_of_threads, &magic_threads, num_of_threads);

    for (int i = 0; i < num_of_threads; i++){

        data[i].from = i * (RANGE / num_of_threads);
        data[i].to = data[i].from + (RANGE / num_of_threads);
        data[i].thread_num = (num_of_magic_thr + i) % num_of_log_CPU;

        if (pthread_create(thr_info + i, NULL, thread_body, data + i) != 0) CHECK_ERROR("thread create:");
    }

    wait_for_magic(num_of_magic_thr, magic_threads);
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

    pthread_t* thr_info = (pthread_t*) calloc(num_of_threads, sizeof(pthread_t));
    if (thr_info == NULL) CHECK_ERROR("alloc threads:");

    struct thread_data* data = (struct thread_data*) calloc(num_of_threads, sizeof(struct thread_data));
    if (data == NULL) CHECK_ERROR("alloc data:");

    set_threads(num_of_threads, thr_info, data);
    get_result(num_of_threads, thr_info, data);

    free(thr_info);
    free(data);
}
