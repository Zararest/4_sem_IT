#include "./lib/Connection.h"
#include "./lib/Integral.h"

int connect_to_server(int num_of_threads){  

    DEBUG_PRINT("in connection");

    struct ServAddr* tmp = recv_serv_addr();
    struct sockaddr_in recved_addr = tmp->serv_addr;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    free(tmp);
    
    DEBUG_PRINT("received server addr");

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) CHECK_ERROR("socket:");
    
    addr.sin_family = AF_INET;
    addr.sin_port = recved_addr.sin_port;
    addr.sin_addr = recved_addr.sin_addr;

    #undef ACTION
    #define ACTION close(sock); exit(0);
    
    int connect_ret = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (connect_ret < 0) CHECK_ERROR("connect:");                               //connection refused

    int bytes_sent = send(sock, &num_of_threads, sizeof(int), 0);
    if (bytes_sent != sizeof(int)) CHECK_ERROR("send number of threads:");

    #undef ACTION
    #define ACTION exit(0);

    return sock;
}

Task* receive_task(int sock){

    Task* task = create_task(0, 0, 0);

    int bytes_read = recv(sock, task, sizeof(Task), 0);
    if (bytes_read != sizeof(Task)) CHECK_ERROR("receive task:");

    return task;
}

double complete_task(Task* task){

    double from, to;
    int num_of_threads;
    get_values(&from, &to, &num_of_threads, task);

    if (num_of_threads == 0){

        return 0;
    }

    Integral* main_integr = create_integral(num_of_threads, (to - from) / num_of_threads, from);
    int num_of_log_CPU = get_nprocs();

    Integral* magic = NULL;

    if (num_of_log_CPU > num_of_threads){

        magic = create_integral(num_of_log_CPU - num_of_threads, (to - from) / num_of_threads, from);
    }

    set_threads(main_integr, 0);

    if (magic != NULL){

        set_threads(magic, num_of_threads);
        get_result(magic);
    }

    double result = get_result(main_integr);

    clean_integral(main_integr);
    clean_integral(magic);

    return result;
}

void send_result(double value, clock_t time, int sock){

    Result* result = create_result(value, time);

    int bytes_sent = send(sock, result, sizeof(Result), 0);
    if (bytes_sent != sizeof(Result)) CHECK_ERROR("send result:");

    free_result(result);

    close(sock);
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

    clock_t start, end;
    int sock = connect_to_server(num_of_threads);
    Task* task = receive_task(sock);

    start = clock();
    double value = complete_task(task);
    end = clock();

    send_result(value, end - start, sock);
    free_task(task);
}

