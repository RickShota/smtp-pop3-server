/**
 * @file net.h
 * @author 黄瑞
 * @date 2024.3.20
 * @details 网络套接字模块头文件
*/
#ifndef NET_H
#define NET_H

#include "common.h"
// smtp锁步
int handleConnection(int sockfd, table_t *table, mail_t *pmail);

// pop3锁步
int pop3Connection(int cfd, sub_t *subject, table_t *p, mail_t *pmail);

#endif