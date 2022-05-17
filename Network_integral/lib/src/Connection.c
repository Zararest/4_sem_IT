#include "../Connection.h"

Task* create_task(double from, double to, int num_of_threads){

    Task* task = (Task*) calloc(1, sizeof(Task));
    if (task == NULL) CHECK_ERROR("task create:");

    task->from = from;
    task->to = to;
    task->num_of_threads = num_of_threads;

    return task;
}

void fill_task(double from, double to, int num_of_threads, Task* task){

    if (task == NULL){

        fprintf(stderr, "fill_task wrong params\n");
        return;
    }

    task->from = from;
    task->to = to;
    task->num_of_threads = num_of_threads;
}

void get_values(double* from, double* to, int* num_of_threads, Task* task){

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
        return -1;
    }

    return result->value;
}

void free_result(Result* result){

    free(result);
}

void send_serv_addr(ServAddr* serv_addr){

    if (serv_addr == NULL){

        fprintf(stderr, "send_serv_addr wrong params\n");
        exit(0);
    }

    struct sockaddr_in addr = {0};
    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT_NUM);
    addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) CHECK_ERROR("UPD socket:");

    #undef ACTION
    #define ACTION close(sock); exit(0);

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable)) != 0)
        CHECK_ERROR("set socket broadcast:");

    if (sendto(sock, serv_addr, sizeof(ServAddr), 0, (struct sockaddr*) &addr, sizeof(addr)) != sizeof(ServAddr))
        CHECK_ERROR("send server adress:");

    #undef ACTION
    #define ACTION exit(0);
}

ServAddr* recv_serv_addr(){

    ServAddr* remote_serv = (ServAddr*) calloc(1, sizeof(ServAddr));

    if (remote_serv == NULL){

        fprintf(stderr, "can't allocate remote server addr\n");
        exit(0);
    }
 
    struct sockaddr_in serv_addr = {0};
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(UDP_PORT_NUM);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) CHECK_ERROR("recv UDP socket:");

    #undef ACTION
    #define ACTION close(sock); exit(0);

    if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) != 0)
        CHECK_ERROR("bind in recv UDP:");

    DEBUG_PRINT("before server receiving");

    if (recvfrom(sock, remote_serv, sizeof(ServAddr), 0, NULL, NULL) != sizeof(ServAddr))
        CHECK_ERROR("recv remote serv addr:");

    #undef ACTION
    #define ACTION exit(0);
    
    DEBUG_PRINT("server received");

    close(sock);

    return remote_serv;    
}