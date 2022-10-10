CC=gcc

all: server.out client.out

server.out: server.c
	$(CC) server.c -o server.out -lsocket -lnsl

client.out: client.c
	$(CC) client.c -o client.out -lsocket -lnsl

