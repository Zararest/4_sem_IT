#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUM_OF_COMMANDS 100000
#define LINE_SIZE   6
#define ADD_ELEM    "a"
#define GET_ELEM    "g"
#define REMOVE_ELEM "r"

void generate_line(FILE* out){

    putc(' ', out);

    for (int i = 0; i < LINE_SIZE; i++){

        unsigned cur_char = 'a' + (rand() % 10);
        putc(cur_char, out);
    }

    putc(' ', out);
}

int main(int argc, char** argv){

    if (argc < 2){

        printf("Too few parametrs\n");
        return 0;
    }

    FILE* out = fopen(argv[1], "w");
    srand(time(NULL));

    if (out == NULL){

        printf("Can't open file\n");
        return 0;
    }

    fprintf(out, "%i\n", NUM_OF_COMMANDS);

    for (int i = 0; i < NUM_OF_COMMANDS; i++){

        unsigned cur_command = rand() % 10;

        if (cur_command < 4){

            fprintf(out, "a %i ", LINE_SIZE + 1);
        }  

        if (cur_command >= 4 && cur_command < 8){

            fprintf(out, "g %i ", LINE_SIZE + 1);
        }

        if (cur_command >= 8){

            fprintf(out, "r %i ", LINE_SIZE + 1);
        }

        generate_line(out);

        putc('\n', out);
    }
}