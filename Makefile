CC = gcc

FLAGS = -Wall

compile: server.c
	$(CC) $(FLAGS) -o server server.c 

run: main.c
	./server
