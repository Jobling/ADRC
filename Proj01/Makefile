CC = gcc
FLAGS = -g -Wall

default: main.o tree.o helper.o
	@echo
	@echo "Creating executable BinTree"
	@echo
	@$(CC) $(FLAGS) -o BinTree main.o tree.o helper.o
	
main.o: main.c tree.h
	@$(CC) $(FLAGS) -c main.c
	
tree.o: tree.c tree.h
	@$(CC) $(FLAGS) -c tree.c
	
helper.o: helper.c helper.h
	@$(CC) $(FLAGS) -c helper.c
	
clean:
	@echo
	@echo "Removing all .o files"
	@rm *.o
	@echo
