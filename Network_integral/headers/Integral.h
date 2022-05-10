#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <math.h>

#define DELTA_X 0.0002
#define RANGE 10000
#define PAGE_SIZE 4096

struct Integral;

double definite_integral(double from, double to);

double get_result(struct Integral* integr);
void set_threads(struct Integral* integr, int num_of_busy_cores);

struct Integral* create_integral(int num_of_threads, int range_per_thread);
void clean_integral(struct Integral* integr);