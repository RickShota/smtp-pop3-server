/**
 * @file net.h
 * @author 黄瑞
 * @date 2024.3.30
 * @brief 网络套接字模块头文件
*/
#ifndef NET_H
#define NET_H

#include "common.h"
// 处理与客户端的SMTP连接，进行邮件的交互过程
int handleConnection(int sockfd, mail_t *pmail);

// 处理与客户端的POP3连接，进行邮件的收取、删除等操作
int pop3Connection(int cfd, table_t *p);

#endif