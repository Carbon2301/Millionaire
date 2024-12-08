# Define compiler and libraries
CC = gcc
CFLAGS = -pthread
LDFLAGS = -L/usr/local/mysql/lib -lmysqlclient
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

     
TARGET = test           # Tên file đầu ra (chương trình sau khi biên dịch)
SRC = test.c            # Tên file nguồn

# Quy tắc mặc định để biên dịch và liên kết chương trình
all: $(TARGET)

# Quy tắc biên dịch
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Quy tắc chạy chương trình
run: $(TARGET)
	./$(TARGET)

# Quy tắc dọn dẹp (xóa file đã biên dịch)
clean:
	rm -f $(TARGET)
