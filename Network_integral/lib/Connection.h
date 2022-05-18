#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "Error.h"

#define BACK_LOG 50
#define SERV_PORT_NUM 4901 
#define UDP_PORT_NUM 4902

#define ACCEPT_TIMEOUT_SEC  0       
#define ACCEPT_TIMEOUT_USEC 100000 

#define KEEPCNT 3 
#define KEEPIDLE 2 
#define KEEPINTVL 1

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

typedef struct sockaddr_in ServAddr;

Task* create_task(double from, double to, int num_of_threads);
void fill_task(double from, double to, int num_of_threads, Task* task);
void get_values(double* from, double* to, int* num_of_threads, Task* task);
void free_task(Task* task);

Result* create_result(double value, clock_t time);
void set_value(double value, Result* result);
double get_value(Result* result);
void free_result(Result* result);

void send_serv_addr(ServAddr* serv_addr);
ServAddr* recv_serv_addr();

void set_keep_alive(int sock);

