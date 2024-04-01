/**
 * @file mailrecv.c
 * @author 黄瑞
 * @date 2024.3.21
 * @details 邮件接收-SMTP通讯过程源文件
*/
#include "mailrecv.h"

// 获取用户名并存储
int getUsername(int sockfd, table_t *p) {
  char buf[1024] = "";
  char *str = NULL;
  int len = read(sockfd, buf, sizeof(buf) - 1);
  if(len <= 0) {
    perror("getUsername read error");
    return -1;
  }
  buf[len - 2] = 0; // 去除字符串末尾的换行符
  str = base64_decode(buf); // 解码用户名

  if(verUsername(str, p) != 0) {
    close(sockfd);
    return -1;
  }
  strcpy(p->username, str);
  printf("username: %s\n", p->username);
  return 0;
}

// 获取用户密码并存储
int getPassword(int sockfd, table_t *p) {
  char buf[128] = "";
  char *str;
  int len = read(sockfd, buf, sizeof(buf) - 1);

  buf[len - 2] = 0;
  str = base64_decode(buf);
  if(verPassword(str, p) != 0) {
    close(sockfd);
    return -1;
  }
  // 验证成功，将用户名密码存入到验证表结构体中
  strcpy(p->password, str);
  printf("password: %s\n", p->password);
  return 0;
}

// 读取包含发件人地址并存储
int getFromAddress(int sockfd, mail_t *pmail) {
  char buf[128], temp[128] = "";
  char *addressStart;
  char *addressEnd;
  int addressLen, tempLen = 0;

  int len = read(sockfd, buf, sizeof(buf) - 1);
  //i在buf末尾添加字符串结束符'\0'
  buf[len] = '\0';
  //从buf中查找'<'到'>'之间的数据
  if((addressStart = strchr(buf, '<')) == NULL) {
    perror("find sender address error");
    return -1;
  }
  if((addressEnd = strchr(buf, '>')) == NULL) {
    perror("find delimiter error");
    return -1;
  }
  if((addressLen = addressEnd - addressStart) <= 0) {
    perror("find data error");
    return -1;
  }

  strncpy(temp, addressStart + 1, addressLen - 1);
  temp[addressLen - 1] = '\0';
  tempLen = strlen(temp);
  strncpy(pmail->send, temp, tempLen);
  printf("pmail->send: %s\n", pmail->send);
  return 0;
}

// 读取包含收件人地址并存储
int getToAddress(int sockfd, mail_t *pmail) {
  char buf[128] = "";
  char *start, *end;
  int len = read(sockfd, buf, sizeof(buf) - 1);
  if(len <= 0) {
    perror("read error");
    return -1;
  }
  buf[len] = '\0';
  if((start = strchr(buf, '<')) == NULL) {
    perror("find start error");
    return -1;
  }
  start++;
  if((end = strchr(start, '>')) == NULL) {
    perror("find end error");
    return -1;
  }
  *end = '\0';
  //将截取的地址存入邮件结构体中
  strncpy(pmail->recv, start, sizeof(buf) - 1);
  pmail->recv[sizeof(buf) - 1] = '\0';
  printf("pmail->recv: %s\n", pmail->recv);
  return 0;
}

// 读取邮件正文内容并存储
int getBody(int sockfd, mail_t *pmail) {
  char buf[1024 * 10] = "";
  char temp[1024 * 10] = "";
  int len = read(sockfd, buf, sizeof(buf) - 1);
  if(len < 0) {
    perror("recv error");
    return -1;
  }

  strncpy(temp, buf, sizeof(temp) - 1);
  temp[sizeof(temp) - 1] = '\0';
  strncat(pmail->raw, temp, strlen(temp));
  printf("pmail->raw: %s\n", pmail->raw);


  // 查找temp中是否有filename
  // 如果有则再recv一次接收\r\n.\r\n
  // 否则则调用getSlave再recv一次接收\r\n.\r\n  
  while(1) {
    char *pos = strstr(temp, "filename=");
    if(pos == NULL)
      break;

    pos += strlen("filename=");// 跳过查到的filename=
    char *end = strstr(pos, "\r\n");// 查找\r\n
    if(end == NULL)
      break;
    *end = '\0';
    strncat(pmail->filename, pos, end - pos);// 把附件文件名复制到邮件结构体中	
    getSlave(sockfd, pmail);
    break;
  }

  printf("pmail->filename: %s\n", pmail->filename);
  return 0;
}

// 读取邮件附件内容并存储
int getSlave(int sockfd, mail_t *pmail) {
  char temp[1024 * 10];
  int len;
  char *pos = NULL;

  while((pos = strstr(pmail->raw, "filename=")) != NULL) {
    pos += strlen("filename=");
    // 查找第一个\r\n标记
    char *end = strchr(pos, '\r');
    if(end == NULL)
      break;

    // 查找第二个\r\n标记
    char *end2 = strchr(end + 1, '\r');
    if(end2 == NULL)
      break;

    // 查找第三个\r\n标记
    char *end3 = strstr(end2 + 1, "\r\n\r\n");
    if(end2 == NULL)
      break;

    len = end3 - end2;
    strncpy(temp, end2 + 2, len);
    temp[sizeof(temp) - 1] = '\0';
    strncpy(pmail->attr, temp, sizeof(pmail->recv) - 1);
    break;
  }

  printf("pmail->attr: %s\n", pmail->attr);
  printf("pmail->filename; %s\n", pmail->filename);
  return 0;
}

// 读取POP3协议命令，提取验证用户名并存储
int getUserPop(int sockfd, table_t *p) {
  printf("begin\n");
  char buf[128] = "";
  //memset(buf, 0, sizeof(buf));
  while(!strlen(buf)) {

    read(sockfd, buf, sizeof(buf));
    printf("buf = %s\n", buf);
  }
  printf("read over\n");
  //buf[len] = '\0';

  char *userStart = strstr(buf, "USER ");
  userStart += strlen("USER ");
  //提取用户名
  char str[80];

  strncpy(str, userStart, 10);
  str[10] = '\0';
  printf("username %s\n", str);
  printf("verUsername before\n");

  // 进行用户名验证
  if(verUsername(str, p) < 0) {
    fprintf(stderr, "username diff\n");
    return -1;
  }

  printf("verUsername after\n");
  //将用户名复制到验证表结构体 table中
  strcpy(p->username, str);
  return 0;
}

// 读取POP3协议命令，提取验证密码并存储
int getPassPop(int sockfd, table_t *p) {
  char buf[80] = "";
  memset(buf, 0, sizeof(buf));
  int len = read(sockfd, buf, sizeof(buf) - 1);
  while(len == 0) {

    len = read(sockfd, buf, sizeof(buf) - 1);
  }
  buf[len] = '\0';
  char *passStart = strstr(buf, "PASS ");
  passStart += strlen("PASS ");

  char *passEnd = strchr(passStart, '\r');
  if(passEnd == NULL) {
    return -1;
  }

  int passLen = passEnd - passStart;
  char *password = malloc(passLen + 1);
  strncpy(password, passStart, passLen);
  password[passLen] = '\0';

  strncpy(p->password, password, sizeof(p->password));

  free(password);
  return 0;
}