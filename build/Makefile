CC = gcc
CFLAGS = -g 
CPPFLAGS = --coverage

DIR = ../
SRC = $(DIR)main.c $(DIR)HashTable.c
OBJ = HashTable.o main.o

INFONAME = HashCover
INFODIR = ../report

all: $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o main

$(OBJ): $(SRC)
	$(CC) -c $(SRC)


.PHONY: test
test:
	lcov -t $(INFONAME) -o $(INFONAME).info -c -d .

.PHONY: show
show: 
	genhtml -o $(INFODIR) $(INFONAME).info

.PHONY: clean
clean:
	rm -rf *.o *.gcda *.gcno *.info main 