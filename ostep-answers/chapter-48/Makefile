CC := clang
ARGS := -Wall -Wextra

all: client server

client: client.c udp.c udp.h
	$(CC) client.c udp.c -o client $(ARGS)

server: server.c udp.c udp.h
	$(CC) server.c udp.c -o server $(ARGS)

clean:
	rm client
	rm server