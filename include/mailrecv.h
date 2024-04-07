/**
 * @file mailrecv.h
 * @author 黄瑞
 * @date 2024.3.31
 * @brief 邮件接收-SMTP通讯过程头文件
*/
#ifndef MAILRECV_H
#define MAILRECV_H

#include "common.h"
#include "parsemail.h"
#include "base64.h"

/**
 * @brief 获取用户名并存储
 * @param sockfd 套接字
 * @param p 用户表，用于存储用户信息
 * @return int 成功返回 0，否则返回 -1
 */
int getUsername(int sockfd, table_t *p);

/**
 * @brief 获取用户密码并存储
 * @param sockfd 套接字
 * @param p 用户表，用于存储用户信息
 * @return int 成功返回 0，否则返回 -1
 */
int getPassword(int sockfd, table_t *p);

/**
 * @brief 读取包含发件人地址并存储
 * @param sockfd 套接字
 * @param pmail 邮件结构体，用于存储邮件信息
 * @return int 成功返回 0，否则返回 -1
 */
int getFromAddress(int sockfd, mail_t *pmail);

/**
 * @brief 读取包含收件人地址并存储
 * @param sockfd 套接字
 * @param pmail 邮件结构体，用于存储邮件信息
 * @return int 成功返回 0，否则返回 -1
 */
int getToAddress(int sockfd, mail_t *pmail);

/**
 * @brief 读取邮件正文内容并存储
 * @param sockfd 套接字
 * @param pmail 邮件结构体，用于存储邮件信息
 * @return int 成功返回 0，否则返回 -1
 */
int getBody(int sockfd, mail_t *pmail);

/**
 * @brief 读取邮件附件内容并存储
 * @param sockfd 套接字
 * @param pmail 邮件结构体，用于存储邮件信息
 * @return int 成功返回 0，否则返回 -1
 */
int getSlave(int sockfd, mail_t *pmail);

/**
 * @brief 读取POP3协议命令，提取验证用户名并存储
 * @param sockfd 套接字
 * @param p 用户表，用于存储用户信息
 * @return int 成功返回 0，否则返回 -1
 */
int getUserPop(int sockfd, table_t *p);

/**
 * @brief 读取POP3协议命令，提取验证密码并存储
 * @param sockfd 套接字
 * @param p 用户表，用于存储用户信息
 * @return int 成功返回 0，否则返回 -1
 */
int getPassPop(int sockfd, table_t *p);

#endif
