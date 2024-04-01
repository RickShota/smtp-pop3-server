/**
 * @file base64.h
 * @author 黄瑞
 * @date 2024.3.21
 * @details base64编码解码头文件
*/
#ifndef BASE64_H
#define BASE64_H

#include "common.h"

#define BASE64_PAD64 '='

/**
 * @details 字符串 => BASE64编码
 * @param data 待编码的原始字符串(必须是以null结尾的C字符串)
 * @return 指向BASE64编码后的字符串的指针
 */
char *base64_encode(const char *data);

/**
 * @details BASE64编码 => 字符串
 * @param bdata 待解码的BASE64编码字符串(必须是以null结尾的C字符串)
 * @return 指向解码后的原始数据的字符串指针
 */
char *base64_decode(const char *bdata);

#endif
