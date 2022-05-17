#include <error.h>

#define ACTION exit(0)

#define CHECK_ERROR(str)    do{             \
                                perror(str);\
                                ACTION;     \
                            } while (0)  
