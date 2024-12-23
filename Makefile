# Define compiler and libraries
CC = gcc
CFLAGS = -pthread
LDFLAGS = -L/usr/local/mysql/lib -lmysqlclient -lcrypto
CLIENT_SRC = client.c
SERVER_SRC = server.c

# Define targets
all: server client

# Compile server
server: $(SERVER_SRC)
	$(CC) $(CFLAGS) -o server $(SERVER_SRC) $(LDFLAGS)

# Compile client
client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o client $(CLIENT_SRC)

# Clean up generated files
clean: 
	rm -f server client
