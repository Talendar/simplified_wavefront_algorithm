run: program
	./program

all: clean main.o board.o
	gcc board.o main.o -o program

main.o: main.c
	gcc -c main.c

board.o: board.c board.h
	gcc -c board.c

clean:
	rm -rf *.o program
