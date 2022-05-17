#include "./lib/Connection.h"
#include "./lib/Integral.h"

typedef struct Computers{

    int num_of_computers;
    int* sockets;
} Computers;

Computers* init_computers(int num_of_computers){ //ok

    Computers* ret_val = (Computers*) calloc(1, sizeof(Computers));
    if (ret_val == NULL) CHECK_ERROR("computers init:");

    ret_val->sockets = (int*) calloc(num_of_computers, sizeof(int));
    if (ret_val->sockets == NULL) CHECK_ERROR("sockets init:");

    ret_val->num_of_computers = num_of_computers;

    return ret_val;
}

void delete_computers(Computers* computers){  //ok

    if (computers != NULL){

        if (computers->sockets != NULL){

            for (int i = 0; i < computers->num_of_computers; i++) 
                close(computers->sockets[i]);      
        }

        free(computers->sockets);
        free(computers);
    }
}

int new_computer(int* socket, int listener){ //ok

    *socket = accept(listener, NULL, NULL);
    if (*socket < 0) CHECK_ERROR("accept:");
    
    int num_of_threads, bytes_read;
    bytes_read = recv(*socket, &num_of_threads, sizeof(int), 0);
    if (bytes_read != sizeof(int)) CHECK_ERROR("getting the number of threads:");

    if (num_of_threads < 1){

        fprintf(stderr, "Incorrect value of the number of threads\n");
        exit(0);
    }

    return num_of_threads;
}


int connect_computers(Computers* computers){ //ok

    ServAddr TCP_serv_addr;

    int listener, num_of_threads = 0;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    listener = socket(AF_INET, SOCK_STREAM, 0);  
    if (listener < 0) CHECK_ERROR("listener:");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);      

    #undef ACTION
    #define ACTION close(listener); exit(0);

    int bind_ret = bind(listener, (struct sockaddr*) &addr, sizeof(addr));
    if (bind_ret < 0) CHECK_ERROR("bind:");

    int listen_ret = listen(listener, BACK_LOG);
    if (listen_ret < 0) CHECK_ERROR("listen:");

    #undef ACTION
    #define ACTION exit(0);

    TCP_serv_addr.serv_addr = addr;
    int UDP_fd = send_serv_addr(&TCP_serv_addr);                 //отправляем адрес
    
    DEBUG_PRINT("after sending TCP address");

    for (int i = 0; i < computers->num_of_computers; i++){

        num_of_threads += new_computer(&computers->sockets[i], listener);
    }

    close(listener);                         //?
    close(UDP_fd);                                      //закрыли отправление адреса

    return num_of_threads;
}

double calc_new_task(int num_of_computers, int cur_computer, int num_of_threads, double start_point, Task* task){

    double range_per_thread = RANGE / num_of_threads;
    int cur_num_of_threads = num_of_threads / num_of_computers;

    if (cur_computer < (num_of_threads % num_of_computers)){

        cur_num_of_threads++;
    }

    double cur_range = cur_num_of_threads * range_per_thread;

    fill_task(start_point, start_point + cur_range, cur_num_of_threads, task);

    return start_point + cur_range;
}

void distribute_resources(Computers* computers, int num_of_threads){ //ok

    Task* task = create_task(0, 0, 0);
    double start_point = 0;

    for (int i = 0; i < computers->num_of_computers; i++){

        start_point = calc_new_task(computers->num_of_computers, i, num_of_threads, start_point, task);

        int bytes_sent = send(computers->sockets[i], task, sizeof(Task), 0);
        if (bytes_sent != sizeof(Task)) CHECK_ERROR("send task:");
    }

    free_task(task);
}

double get_results(Computers* computers){ //ok

    Result* result = create_result(0, 0);
    double integr_value = 0;

    for (int i = 0; i < computers->num_of_computers; i++){

        int bytes_read = recv(computers->sockets[i], result, sizeof(Result), 0);
        if (bytes_read != sizeof(Result)) CHECK_ERROR("recv result:");

        integr_value += get_value(result);
    }

    free_result(result);

    return integr_value;
}


int main(int argc, char** argv){

    if (argc < 2){
        
        printf("Not enough parameters\n");
        return 0;
    }

    int num_of_computers = atoi(argv[1]);

    if (num_of_computers < 1){

        printf("Not enough computers\n");
        return 0;
    }

    double result;
    int connected_computers = 0;
    
    Computers* computers = init_computers(num_of_computers);

    int num_of_threads = connect_computers(computers);
    distribute_resources(computers, num_of_threads);
    result = get_results(computers);

    delete_computers(computers);

    printf("result: %.2lf\n", result);
}