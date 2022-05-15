#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>

#define BACK_LOG 50
#define PORT_NUM 4111

typedef struct Task{

    int num_of_threads;
    double from;
    double to;
} Task;

typedef struct Result{

    double value;
    clock_t time;
} Result;

Task* create_task(double from, double to, int num_of_threads);
void fill_task(double from, double to, double num_of_threads, Task* task);
void get_values(double* from, double* to, double* num_of_threads, Task* task);
void free_task(Task* task);

Result* create_result(double value, clock_t time);
void set_value(double value, Result* result);
double get_value(Result* result);
void free_result(Result* result);

