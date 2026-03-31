# Khai báo trình biên dịch và flag
CC = gcc
CFLAGS = -Wall -Wextra -g -MMD

# Tên các file thực thi
SERVER_TARGET = server
CLIENT_TARGET = client

# File nguồn tương ứng
SERVER_SRCS = server.c
CLIENT_SRCS = client.c

# Rule mặc định: Biên dịch cả hai
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Biên dịch Server
$(SERVER_TARGET): $(SERVER_SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $@ $^

# Biên dịch Client
$(CLIENT_TARGET): $(CLIENT_SRCS:.c=.o)
	$(CC) $(CFLAGS) -o $@ $^

# Rule biên dịch chung .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Các lệnh chạy riêng biệt
# 'make run-server' để chạy server
run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

# 'make run-client' để chạy client
run-client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

# dọn dẹp
clean:
	rm -f *.o *.d $(SERVER_TARGET) $(CLIENT_TARGET)

# Tự động chèn file phụ thuộc
-include *.d
.PHONY: all clean run-server run-client
