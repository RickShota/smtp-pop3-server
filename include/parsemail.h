/**
 * @file parsemail.h
 * @author 黄瑞
 * @date 2024.3.30
 * @details 邮件解析模块头文件
*/
#ifndef PARSEMAIL_H
#define PARSEMAIL_H

#include "common.h"

int verUsername(const char *username, table_t *userTable);

int verPassword(const char *password, table_t *userTable);

int parseMail(struct mail *pmail, sub_t *subject);


#endif