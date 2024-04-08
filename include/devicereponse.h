/**
 * @file deviceponse.h
 * @author 黄瑞
 * @date 2024.4.3
 * @brief 控制响应模块头文件
*/
#include "common.h"

/**
 * @brief 向套接字发送邮件
 * @param sockfd 套接字
 * @param mailName 要发送的邮件文件路径
 * @return int 成功返回 0，否则返回 -1
 */
int sendMail(int sockfd, const char *mailName);