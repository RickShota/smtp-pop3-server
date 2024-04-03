/**
 * @file devicectrl.h
 * @author 黄瑞
 * @date 2024.4.1
 * @details 设备控制模块头文件
*/
#ifndef DEVICECTRL_H
#define DEVICECTRL_H

#include "common.h"
#include "mailrecv.h"

// 处理邮件对象和主题控制结构
int subjectControl(const mail_t *pmail, sub_t *subject);

// 执行更新操作，将邮件附件内容写入"table.txt"文件中
int emitUpdate(const mail_t *pmail, sub_t *subject);

// 根据主题控制结构中的命令执行相应的控制操作
int emitCommand(sub_t *subject);

// 生成一个新的邮件文件名
char *getCreatMailName(const char *userName, char *mailName);

// 获取要发送的邮件文件名
char *getSendMailName(const char *userName, char *mailName);

// 创建邮件文件并发送邮件内容到指定的套接字
int createMail(int sockfd, const mail_t *pmail, sub_t *subject);

// 获取当前本地时间，并格式化为字符串
char *getCurtime();


#endif