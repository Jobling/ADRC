CC = gcc
FLAGS = -g -Wall

default: main.o
	@echo "Creating executable Flux"
	$(CC) $(FLAGS) -o Flux main.o
	
main.o: main.c 
	$(CC) $(FLAGS) -c main.c

clean:
	@echo "Removing all .o files"
	rm *.o
