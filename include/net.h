/**
 * @file net.h
 * @author 黄瑞
 * @date 2024.3.30
 * @brief 网络套接字模块头文件
*/
#ifndef NET_H
#define NET_H

#include "common.h"
/**
 * @brief 处理与客户端的SMTP连接，进行邮件的交互过程
 * @param sockfd 套接字
 * @param pmail 邮件结构体，用于获取邮件信息
 * @return int 成功返回 0，否则返回 -1
 */
int handleConnection(int sockfd, mail_t *pmail);

/**
 * @brief 处理与客户端的POP3连接，进行邮件的收取、删除等操作
 * @param cfd 套接字
 * @param p 用户表，用于存储用户信息
 * @return int 成功返回 0，否则返回 -1
 */
int pop3Connection(int cfd, table_t *p);

#endif