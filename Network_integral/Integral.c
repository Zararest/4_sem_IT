#include "./headers/Integral.h"

#define DATA_SIZE sizeof(double) * 2 + sizeof(int) + sizeof(pthread_t)

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                exit(0);    \
                            } while (0)  

struct thread_data{

    pthread_t thr_info;
    double from;
    double to;
    int thread_num;
    double result[(PAGE_SIZE - DATA_SIZE) / sizeof(double)];
};

struct Integral{

    struct thread_data* thr_arr;
    int num_of_threads;
};

double func(double x){

    return pow(sin(x), 2) + pow(cos(x) * cos(x), 3.7);
}

double definite_integral(double from, double to){
    
    double res = 0;

    for (double x = from; x < to; x += DELTA_X){

        res += func(x) * DELTA_X;
    }
    
    return res; 
}

void* thread_body(void* args){

    struct thread_data* data = (struct thread_data*) args;
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(data->thread_num, &cpuset);
    
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    
    *data->result = definite_integral(data->from, data->to);

    return NULL;
}

double get_result(struct Integral* integr){

    double integral = 0;

    for (int i = 0; i < integr->num_of_threads; i++){

        if (pthread_join(integr->thr_arr[i].thr_info, NULL) != 0) 
            CHECK_ERROR("thread join:");

        integral += integr->thr_arr[i].result[0];
    }

    return integral;
}

void set_threads(struct Integral* integr, int num_of_busy_cores){

    int num_of_log_CPU = get_nprocs();

    for (int i = 0; i < integr->num_of_threads; i++){
        
        struct thread_data* cur_data = integr->thr_arr + i;
        pthread_t* cur_thr_info = &cur_data->thr_info;

        cur_data->thread_num = (num_of_busy_cores + i) % num_of_log_CPU;

        if (pthread_create(cur_thr_info, NULL, thread_body, cur_data) != 0)
            CHECK_ERROR("thread create:");
    }
}

struct Integral* create_integral(int num_of_threads, int range_per_thread){
    
    struct Integral* integr = (struct Integral*) calloc(1, sizeof(struct Integral));
    integr->thr_arr = (struct thread_data*) calloc(num_of_threads, sizeof(struct thread_data));
    integr->num_of_threads = num_of_threads;

    for (int i = 0; i < num_of_threads; i++){

        integr->thr_arr[i].from = i * range_per_thread;
        integr->thr_arr[i].to = (i + 1) * range_per_thread;
    }

    return integr;
}

void clean_integral(struct Integral* integr){

    if (integr != NULL)
        free(integr->thr_arr);
        
    free(integr);
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

    struct Integral* main_integr = create_integral(num_of_threads, RANGE / num_of_threads);
    int num_of_log_CPU = get_nprocs();

    struct Integral* magic = NULL;

    if (num_of_log_CPU > num_of_threads){

        magic = create_integral(num_of_log_CPU - num_of_threads, RANGE / num_of_threads);
    }

    set_threads(main_integr, 0);

    if (magic != NULL){

        set_threads(magic, num_of_threads);
        get_result(magic);
    }

    get_result(main_integr);

    clean_integral(main_integr);
    clean_integral(magic);
}