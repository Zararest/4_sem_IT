CC = gcc
CFLAGS = -g 
CPPFLAGS = --coverage

DIR = ../
SRC = $(DIR)main.c $(DIR)HashTable.c $(DIR)TestTable.c $(DIR)test.c
SRC_TEST = $(DIR)HashTable.c $(DIR)test.c $(DIR)TestTable.c
OBJ = HashTable.o main.o TestTable.o test.o

INFONAME = HashCover
INFODIR = ../report

$(OBJ): $(SRC)
	$(CC) -c $(SRC)

MainTable: $(OBJ)
	$(CC)  $(OBJ) -o main

TestTable: $(SRC_TEST)
	$(CC) $(CPPFLAGS) $(SRC_TEST) -DTEST -o  test


.PHONY: test
test:
	lcov -t $(INFONAME) -o $(INFONAME).info -c -d .

.PHONY: show
show: 
	genhtml -o $(INFODIR) $(INFONAME).info

.PHONY: TestCoverage
TestCoverage:
	make TestTable
	./test 2> err 
	make test
	make show

.PHONY: clean
clean:
	rm -rf *.o *.gcda *.gcno *.info main test ../report