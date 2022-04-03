CC = gcc
CPPFLAGS = -DTEST -MMD -MP

OBJ = test.o TestTable.o
SRC = test.c TestTable.c 

OBJ_COVERAGE = HashTable.o
SRC_COVERAGE = HashTable.c

INFONAME = HashCover
INFODIR = ./report

all: $(OBJ) $(OBJ_COVERAGE)
	$(CC) $(OBJ) $(OBJ_COVERAGE) -lgcov -o  test 

$(OBJ_COVERAGE): $(SRC_COVERAGE)
	$(CC) $(CPPFLAGS) --coverage -c $(SRC_COVERAGE)

$(OBJ): $(SRC)
	$(CC) $(CPPFLAGS) -c $(SRC)

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