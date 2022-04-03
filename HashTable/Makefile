CC = gcc
CPPFLAGS = -DTEST

OBJ = test.o HashTable.o TestTable.o
SRC = test.c HashTable.c TestTable.c 

INFONAME = HashCover
INFODIR = ./report

all: $(OBJ)
	$(CC) $(OBJ) -lgcov  -o  test 

$(OBJ): $(SRC)
	$(CC) $(CPPFLAGS) --coverage -c $(SRC)

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