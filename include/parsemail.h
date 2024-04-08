/**
 * @file parsemail.h
 * @author 黄瑞
 * @date 2024.3.30
 * @brief 邮件解析模块头文件
*/
#ifndef PARSEMAIL_H
#define PARSEMAIL_H
#include "common.h"

/**
 * @brief 验证用户名并获取相应用户信息
 * @param username 待验证的用户名
 * @param userTable 指向 table_t 结构体的指针，用于存储用户信息
 * @return int 验证成功返回 0，否则返回 -1
 */
int verUsername(const char *username, table_t *userTable);

/**
 * @brief 验证密码并获取相应用户信息
 * @param password 待验证的密码
 * @param userTable 指向 table_t 结构体的指针，用于存储用户信息
 * @return int 验证成功返回 0，否则返回 -1
 */
int verPassword(const char *password, table_t *userTable);

/**
 * @brief 解析邮件内容并提取主题信息
 * @param pmail 指向 mail 结构体的指针，表示待解析的邮件内容
 * @param subject 指向 sub_t 结构体的指针，用于存储解析得到的主题信息
 * @return int 解析成功返回 0，否则返回 -1
 */
int parseMail(struct mail *pmail, sub_t *subject);


#endif