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

    struct sockaddr_in broadcast_addr = {0};
    memset(&broadcast_addr, 0, sizeof(struct sockaddr_in));

    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(UDP_PORT_NUM);
    broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;//htonl(INADDR_BROADCAST);
    //inet_aton ("192.168.31.255", &addr.sin_addr);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) CHECK_ERROR("UPD socket:");

    #undef ACTION
    #define ACTION close(sock); exit(0);

    int enable = 1, bc_size = sizeof(broadcast_addr);

    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable)) != 0) 
        CHECK_ERROR("set socket broadcast:");

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) != 0) 
        CHECK_ERROR("set socket broadcast:");

    if (bind(sock, (struct sockaddr*) &broadcast_addr, bc_size) != 0)
        CHECK_ERROR("bind broadcast:");

    if (sendto(sock, serv_addr, sizeof(ServAddr), 0, (struct sockaddr*) &broadcast_addr, bc_size) != sizeof(ServAddr))
        CHECK_ERROR("send server adress:");

    #undef ACTION
    #define ACTION exit(0);

    close(sock);
}

ServAddr* recv_serv_addr(){

    ServAddr* remote_serv = (ServAddr*) calloc(1, sizeof(ServAddr));

    if (remote_serv == NULL){

        fprintf(stderr, "can't allocate remote server addr\n");
        exit(0);
    }
    
    ServAddr msg;
    struct sockaddr_in serv_addr = {0};
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(UDP_PORT_NUM);
    serv_addr.sin_addr.s_addr = INADDR_ANY;//htonl(INADDR_ANY);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) CHECK_ERROR("recv UDP socket:");

    #undef ACTION
    #define ACTION close(sock); exit(0);

    int enable = 1;
    socklen_t remote_serv_len = sizeof(struct sockaddr);

    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
        CHECK_ERROR("set SO_REUSEADDR in recv:");

    if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) != 0)
        CHECK_ERROR("bind in recv UDP:");

    DEBUG_PRINT("before server receiving");

    if (recvfrom(sock, &msg, sizeof(ServAddr), 0, (struct sockaddr*) remote_serv, &remote_serv_len) != sizeof(ServAddr)) //мы получаем адрес отправителя
        CHECK_ERROR("recv remote serv addr:");

    printf("received remote serv len %i and ServAddr is %li\n", remote_serv_len, sizeof(ServAddr));
    printf("serv_addr: %s\n", inet_ntoa(remote_serv->sin_addr));

    #undef ACTION
    #define ACTION exit(0);
    
    DEBUG_PRINT("server received");

    close(sock);

    return remote_serv;    
}

void set_keep_alive(int sock){

    struct timeval timeout_accept;
    timeout_accept.tv_sec = ACCEPT_TIMEOUT_SEC;
    timeout_accept.tv_usec = ACCEPT_TIMEOUT_USEC;

    int keep_cnt = KEEPCNT;
    int keep_idle = KEEPIDLE;
    int keep_intvl = KEEPINTVL;

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
        CHECK_ERROR("set SO_REUSEADDR:");

    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout_accept, sizeof(timeout_accept)) != 0)
        CHECK_ERROR("set SO_RCVTIMEO:");

    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(int)) != 0)
        CHECK_ERROR("set SO_KEEPALIVE:");

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keep_cnt, sizeof(int)) != 0)
        CHECK_ERROR("set TCP_KEEPCNT:");

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(int)) != 0)
        CHECK_ERROR("set TCP_KEEPIDLE:");

    if (setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keep_intvl, sizeof(int)) != 0)
        CHECK_ERROR("set TCP_KEEPINTVL:");    
}