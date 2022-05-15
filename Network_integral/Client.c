#include "./headers/Connection.h"
#include "./headers/Integral.h"

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)  

int connect_to_server(int num_of_threads){

    struct sockaddr_in addr;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) CHECK_ERROR("socket:");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int connect_ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (connect_ret < 0) CHECK_ERROR("connect:");

    int bytes_sent = send(sock, &num_of_threads, sizeof(int), NULL);
    if (bytes_sent != sizeof(int)) CHECK_ERROR("send number of threads:");

    return sock;
}

Task* receive_task(int sock){

    Task* task = create_task(0, 0, 0);

    int bytes_read = recv(sock, task, sizeof(Task), NULL);
    if (bytes_read != sizeof(Task)) CHECK_ERROR("receive task:");

    return task;
}

double complete_task(Task* task){

    double from, to;
    int num_of_threads;
    get_values(&from, &to, &num_of_threads, task);

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

    int bytes_sent = send(sock, result, sizeof(Result), NULL);
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
    Task* task = receive_task(sock); //может быть нулем !!!!!!!!!!!!!!!!

    start = clock();
    double value = complete_task(task);
    end = clock();

    send_result(value, time, sock);
    free_task(task);
}

