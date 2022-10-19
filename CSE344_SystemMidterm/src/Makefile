
CC = gcc
CFLAGS = -ansi -Wall -std=gnu99 -lm -lpthread -lrt

makefile: ./serverY.c ./become_daemon.c ./arg_serverY.c ./clientX.c
	$(CC) -o serverY ./serverY.c ./become_daemon.c ./arg_serverY.c $(CFLAGS)
	$(CC) -o client clientX.c $(CFLAGS)


