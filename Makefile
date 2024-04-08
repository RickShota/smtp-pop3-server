CC = gcc
DB = gdb
CFLAGS = -Wall -Iinclude -lpthread -pthread

# 目标文件夹
BUILD_DIR = build
SRC_DIR = src

# 目标文件
TARGET1 = $(BUILD_DIR)/smtp.o
TARGET2 = $(BUILD_DIR)/pop3.o

# smtp源文件
SRCS = $(wildcard $(SRC_DIR)/*.c) smtp_main.c

# pop3源文件
POP_SRCS = $(wildcard $(SRC_DIR)/*.c) pop3_main.c

the_main:
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET1)
	$(CC) $(CFLAGS) $(POP_SRCS) -o $(TARGET2)

smtp: the_main
	$(TARGET1)

pop: the_main
	$(TARGET2)

debug:
	$(CC) $(CFLAGS) $(SRCS) -g
	$(DB) ./a.out