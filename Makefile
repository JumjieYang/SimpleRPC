CC=gcc
CFLAGS=-fsanitize=signed-integer-overflow -fsanitize=undefined -g -std=gnu99 -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow

CLIENT=frontend
SERVER=backend

rpc: network.c backend.c frontend.c
	$(CC) -o $(CLIENT) $(CFLAGS) frontend.c network.c
	$(CC) -o $(SERVER) $(CFLAGS) backend.c network.c

clean:
	rm -rf $(CLIENT) $(SERVER)

