CC = gcc
DB = gdb
CFLAGS = -Wall -Iinclude -lpthread

# 目标文件夹
BUILD_DIR = build
SRC_DIR = src

# 目标文件
TARGET = $(BUILD_DIR)/main.o

# 源文件
SRCS = $(wildcard $(SRC_DIR)/*.c)

the_main:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: the_main
	$(TARGET)

test:
	$(CC) $(CFLAGS) $(SRCS) -g
	$(DB) ./a.out