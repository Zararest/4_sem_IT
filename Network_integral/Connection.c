#include "./headers/Connection.h"

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)  

Task* create_task(double from, double to, int num_of_threads){

    Task* task = (Task*) calloc(1, sizeof(Task));
    if (task == NULL) CHECK_ERROR("task create:");

    task->from = from;
    task->to = to;
    task->num_of_threads = num_of_threads;

    return task;
}

void fill_task(double from, double to, double num_of_threads, Task* task){

    if (task == NULL){

        fprintf(stderr, "fill_task wrong params\n");
        return;
    }

    task->from = from;
    task->to = to;
    task->num_of_threads = num_of_threads;
}

void get_values(double* from, double* to, double* num_of_threads, Task* task){

    if (from == NULL || to == NULL || num_of_threads == NULL || task == NULL){

        fprintf(stderr, "get_values wrong params\n");
        return;
    }        

    *from = task->from;
    *to = task->to;
    *num_of_threads = task->num_of_threads;
}

void free_task(Task* task){

    free(task);
}

Result* create_result(double value, clock_t time){

    Result* result = (Result*) calloc(1, sizeof(Result));
    if (result == NULL) CHECK_ERROR("result create:");

    result->time = time;
    result->value = value;

    return result;
}

void set_value(double value, Result* result){

    if (result == NULL){

        fprintf(stderr, "set_value wrong params\n");
        return;
    }

    result->value = value;
}

double get_value(Result* result){

    if (result == NULL){

        fprintf(stderr, "get_value wrong params\n");
        return;
    }

    return result->value;
}

void free_result(Result* result){

    free(result);
}