/**
 * @file parsemail.h
 * @author 黄瑞
 * @date 2024.3.20
 * @details 邮件解析模块头文件
*/
#ifndef PARSEMAIL_H
#define PARSEMAIL_H

#include "common.h"
#include "base64.h"

char *base64Decode(const char *bdata);

int verUsername(const char *username, table_t *userTable);

int verPassword(const char *password, table_t *userTable);

int parseMail(struct mail *pmail, sub_t *subject);


#endif