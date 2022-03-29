CC = gcc
CFLAGS = -g

SRC = main.c Integral.c
OBJ = main.o Integral.c

all: $(OBJ)
	$(CC)  $(OBJ) -o integral

$(OBJ): $(SRC)
	$(CC) -c $(SRC)

.PHONY: clean
clean:
	rm -rf *.o
