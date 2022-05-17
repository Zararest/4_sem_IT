#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "Error.h"

#define BACK_LOG 50
#define PORT_NUM 9132

#define UDP_PORT_NUM 4000

/*
1) отправляем через datagram нужную информацию 
2) при получении датаграма устанавливаем tcp соединение (надо флаги на keepalive поставить)
3) остальное все есть
*/

typedef struct Task{

    int num_of_threads;
    double from;
    double to;
} Task;

typedef struct Result{

    double value;
    clock_t time;
} Result;

typedef struct ServAddr{

    struct sockaddr_in serv_addr;
} ServAddr;

Task* create_task(double from, double to, int num_of_threads);
void fill_task(double from, double to, int num_of_threads, Task* task);
void get_values(double* from, double* to, int* num_of_threads, Task* task);
void free_task(Task* task);

Result* create_result(double value, clock_t time);
void set_value(double value, Result* result);
double get_value(Result* result);
void free_result(Result* result);

int send_serv_addr(ServAddr* serv_addr);
ServAddr* recv_serv_addr();

void set_keep_alive(int sock);

