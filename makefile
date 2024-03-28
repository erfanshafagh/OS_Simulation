all: OS-Simulation

OS-Simulation: list.o OS-Commands.o OS-main.o
	gcc -Wall -o OS-Simulation list.o OS-Commands.o OS-main.o

OS-main.o: OS-main.c 
	gcc -Wall -c OS-main.c

OS-Commands.o: OS-Commands.c OS-Commands.h
	gcc -Wall -c OS-Commands.c

list.o: list.h list.c
	gcc -w    -c list.c

clean:
	rm -f OS-Simulation *.o
