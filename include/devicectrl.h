/**
 * @file devicectrl.h
 * @author 黄瑞
 * @date 2024.4.1
 * @brief 设备控制模块头文件
*/
#ifndef DEVICECTRL_H
#define DEVICECTRL_H

#include "common.h"
#include "mailrecv.h"

/**
 * @brief 根据指令执行对应的动作
 * @param pmail 指向 mail 结构体的指针，用于获取邮件信息
 * @param subject 指向 sub_t 结构体的指针，用于获取指令
 * @return int 成功返回 0，否则返回 -1
 */
int subjectControl(const mail_t *pmail, sub_t *subject);

/**
 * @brief 执行更新操作，将邮件附件内容写入"table.txt"文件中
 * @param pmail 指向 mail 结构体的指针，用于获取邮件信息
 * @param subject 指向 sub_t 结构体的指针，用于获取指令
 * @return int 成功返回 0，否则返回 -1
 */
int emitUpdate(const mail_t *pmail, sub_t *subject);

/**
 * @brief 根据主题控制结构中的命令执行相应的控制操作
 * @param subject 指向 sub_t 结构体的指针，用于获取指令
 * @return int 成功返回 0，否则返回 -1
 */
int emitCommand(sub_t *subject);

/**
 * @brief 生成一个新的邮件文件名
 * @param userName 用户名
 * @param mailName 邮件文件名
 * @return 成功返回文件名字符串指针，否则返回NULL
 */
char *getCreatMailName(const char *userName, char *mailName);

/**
 * @brief 获取要发送的邮件文件名
 * @param userName 用户名
 * @param mailName 邮件文件名
 * @return 成功返回文件名字符串指针，否则返回NULL
 */
char *getSendMailName(const char *userName, char *mailName);

/**
 * @brief 创建邮件文件并发送邮件内容到指定的套接字
 * @param sockfd 套接字
 * @param pmail 邮件结构体指针
 * @param subject 主题结构体指针
 * @return int 成功返回 0，否则返回 -1
 */
int createMail(int sockfd, const mail_t *pmail, sub_t *subject);

/**
 * @brief 获取当前本地时间，并格式化为字符串
 * @return 成功返回字符串指针，否则返回NULL
 */
char *getCurtime();


#endif