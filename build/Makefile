HashTable.o: ../HashTable.c
	gcc -c ../HashTable.c

main.o: ../main.c
	gcc -c ../main.c

main: main.o HashTable.o
	gcc main.o HashTable.o -o main

.PHONY: clean
clean:
	rm -rf *.o main