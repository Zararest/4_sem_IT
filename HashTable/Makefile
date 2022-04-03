CC = gcc
CFLAGS = -g -MD
CPPFLAGS = --coverage

SRC = main.c HashTable.c TestTable.c test.c
OBJ = main.o HashTable.o TestTable.o
SRC_TEST = test.c HashTable.c TestTable.c 

INFONAME = HashCover
INFODIR = ./report

all: $(SRC_TEST)
	$(CC) $(CPPFLAGS) $(SRC_TEST) -MMD -MP -DTEST -o  test 

#сделать раздельную компиляцию 

$(OBJ): $(SRC)
	$(CC) -MMD -MP -c $(SRC)

MainTable: $(OBJ)
	$(CC)  $(OBJ) -o main

.PHONY: test
test:
	./test
	lcov -t $(INFONAME) -o $(INFONAME).info -c -d .
	genhtml -o $(INFODIR) $(INFONAME).info

.PHONY: Valgrind
Valgrind:
	valgrind ./test

.PHONY: clean
clean:
	rm -rf *.d *.o *.gcda *.gcno *.info main test report/*

-include *.d