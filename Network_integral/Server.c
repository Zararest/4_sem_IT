#include "./headers/Connection.h"
#include "./headers/Integral.h"

#include <time.h>

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
                close(computers->sockets[i]);       //?
        }

        free(computers->sockets);
        free(computers);
    }
}

int new_computer(int* socket, int listener){ //ok

    *socket = accept(listener, NULL, NULL);
    if (*socket < 0) CHECK_ERROR("accept:");
    
    int num_of_threads, bytes_read;
    bytes_read = recv(*socket, &num_of_threads, sizeof(int), NULL);
    if (bytes_read != sizeof(int)) CHECK_ERROR("getting the number of threads:");

    if (num_of_threads < 1){

        fprintf(stderr, "Incorrect value of the number of threads\n");
        exit(0);
    }

    return num_of_threads;
}

int connect_computers(Computers* computers){ //ok

    int listener, num_of_threads = 0;
    struct sockaddr_in addr;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) CHECK_ERROR("listener:");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_ret = bind(listener, (struct sockaddr *)&addr, sizeof(addr));
    if (bind_ret < 0) CHECK_ERROR("bind:");

    int listen_ret = listen(listener, BACK_LOG);
    if (listen_ret < 0) CHECK_ERROR(listen_ret);

    for (int i = 0; i < computers->num_of_computers; i++){

        num_of_threads += new_computer(&computers->sockets[i], listener);
    }

    close(listener);                         //?

    return num_of_threads;
}

void calc_new_task(int num_of_computers, int cur_computer, int num_of_threads, Task* task){

}

void distribute_resources(Computers* computers, int num_of_threads){ //ok

    Task* task = create_task(0, 0, 0);

    for (int i = 0; i < computers->num_of_computers; i++){

        calc_new_task(computers->num_of_computers, i, num_of_threads, task);

        int bytes_sent = send(computers->sockets[i], task, sizeof(Task), NULL);
        if (bytes_sent != sizeof(Task)) CHECK_ERROR("send task:");
    }

    free_task(task);
}

void get_results(Computers* computers){ //ok

    Result* result = create_result(0, 0);
    double integr_value = 0;

    for (int i = 0; i < computers->num_of_computers; i++){

        int bytes_read = recv(computers->sockets[i], result, sizeof(Result), NULL);
        if (bytes_read != sizeof(Result)) CHECK_ERROR("recv result:");

        integr_value += get_value(result);
    }

    free_result(result);
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

    clock_t start, end;
    int connected_computers = 0;
    
    Computers* computers = init_computers(num_of_computers);

    int num_of_threads = connect_computers(computers);

    start = clock();
    distribute_resources(computers, num_of_threads);
    get_results(computers);
    end = clock();

    delete_computers(computers);
}