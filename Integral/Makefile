CC = gcc -pthread
CFLAGS = -g

SRC = main.c Integral.c
OBJ = main.o Integral.o

all: $(OBJ)
	$(CC) $(OBJ) -o integral -lm

$(OBJ): $(SRC)
	$(CC) -MMD -MP -c $(SRC)

.PHONY: clean
clean:
	rm -rf *.o *.d

-include *.d
