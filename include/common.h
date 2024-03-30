/**
 * @file common.h
 * @author 黄瑞
 * @date 2024.3.20
 * @details 定义通用结构体和常量头文件
*/
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MAIL (8 * 1024)
#define MAX_ATTACHMENT (7 * 1024)
#define USER_FILE "table.txt"
#define IP "192.168.125.133"
#define SMTP_PORT 10000
#define POP3_PORT 10001

// 邮件结构体定义
typedef struct mail {
  char send[80];	            // 发送者地址
  char recv[80];              // 接收者地址
  char subject[80];	          // 主题字
  char filename[80];	        // 文件名
  char raw[MAX_MAIL];         // 邮件内容
  char attr[MAX_ATTACHMENT];  // 附件内容
  int len;	                  // 附件长度
} mail_t;

// 主题字结构体定义
typedef struct subject_ctl {
  char command[16]; // 命令字
  int bulb;         // 灯的号数(1~8)
  int signal;       // 七段数码管显示信号(0~99)
  int reva;	        // 电机转数(0,1000)
  int bulb_ctl;     // 控制信号（0关灯，1开灯）
  int result;       // 0表示控制成功，－1表示控制失败
} sub_t;

// 验证结构体声明
typedef struct table {
  char username[10]; // 邮件用户名
  char password[10]; // 用户密码
} table_t;


#endif