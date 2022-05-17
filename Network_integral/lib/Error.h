#include <error.h>

#define ACTION exit(0)
#define DEBUG

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                ACTION;     \
                            } while (0)  

#ifdef DEBUG
    #define DEBUG_PRINT(str) printf("DEBUG: %s\n", (str))
#endif 

#ifndef DEBUG
    #define DEBUG_PRINT(str)
#endif