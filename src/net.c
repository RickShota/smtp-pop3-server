/**
 * @file net.c
 * @author 黄瑞
 * @date 2024.3.30
 * @brief 网络套接字模块源文件
*/
#include "net.h"
#include "mailrecv.h"
#include "devicectrl.h"
#include "devicereponse.h"
#include "common.h"

int handleConnection(int sockfd, mail_t *pmail) {
  if(sockfd < 2 || pmail == NULL) {
    perror("handleConnection: args error");
    exit(EXIT_FAILURE);
  }
  printf("开始处理SMTP连接...\n");
  char *response220 = "220 192.168.125.133 ok\r\n";
  char *response250_HELO = "250-192.168.125.133\r\n250-PIPELINING\r\n250-SIZE 52428800\r\n250-AUTH LOGIN PLAIN\r\n250-AUTH=LOGIN\r\n250-MAILCOMPRESS\r\n250 BITMIME\r\n";
  char *response334_user = "334 VXNlcm5hbWU6\r\n";
  char *response334_pass = "334 UGFzc3dvcmQ6\r\n";

  char *response235 = "235 Authenticatin successful\r\n";
  char *response250_ok = "250 OK\r\n";
  char *response_354 = "354 End data with <CR><LF>.<CR><LF>\r\n";
  char *response_221 = "221 Bye\r\n";

  char buf[1024] = "";
  table_t table = { 0 };

  write(sockfd, response220, strlen(response220));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "HELO", 4) && strncmp(buf, "EHLO", 4)) {
    perror("HELO or EHLO error");
    return -1;
  }

  write(sockfd, response250_HELO, strlen(response250_HELO));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "AUTH LOGIN", 10)) {
    perror("AUTH LOGIN error");
    return -1;
  }

  write(sockfd, response334_user, strlen(response334_user));
  if(getUsername(sockfd, &table)) {
    perror("getUsername error");
    return -1;
  }

  write(sockfd, response334_pass, strlen(response334_pass));
  if(getPassword(sockfd, &table)) {
    perror("getPassword error");
    return -1;
  }

  write(sockfd, response235, strlen(response235));
  if(getFromAddress(sockfd, pmail)) {
    perror("getFromAddress error");
    return -1;
  }

  write(sockfd, response250_ok, strlen(response250_ok));
  if(getToAddress(sockfd, pmail)) {
    perror("getToAddress error");
    return -1;
  }

  write(sockfd, response250_ok, strlen(response250_ok));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "DATA", 4)) {
    perror("DATA error");
    return -1;
  }

  write(sockfd, response_354, strlen(response_354));
  if(getBody(sockfd, pmail)) {
    perror("getBody error");
    return -1;
  }

  write(sockfd, response250_ok, strlen(response250_ok));
  sleep(1);
  read(sockfd, buf, sizeof(buf) - 1);
  if(strstr(buf, "QUIT") == NULL) {
    perror("QUIT error");
    return -1;
  }

  write(sockfd, response_221, strlen(response_221));
  printf("SMTP连接处理结束\n\n");
  return 0;
}

int pop3Connection(int sockfd, table_t *table) {
  printf("开始处理pop3连接...\n");

  char *response_server = "+OK Pop3 server\r\n";
  char *response_ok = "+OK \r\n";
  char *response_300 = "+OK 1 300 \r\n";
  char *response_msg = "+OK 1 message\r\n1 300 \r\n.\r\n";
  char *response_120 = "+OK 120 octets \r\n";
  char *response_send = "\r\n.\r\n";

  char buf[1024] = "";

  write(sockfd, response_server, strlen(response_server));
  if(getUserPop(sockfd, table) < 0) {
    perror("getUserPop error");
    return -1;
  }

  write(sockfd, response_ok, strlen(response_ok));
  if(getPassPop(sockfd, table) < 0) {
    perror("getPassPop error");
    return -1;
  }

  write(sockfd, response_ok, strlen(response_ok));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "STAT", 4)) {
    perror("STAT error"); return -1;
  }

  write(sockfd, response_300, strlen(response_300));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "LIST", 4)) {
    perror("LIST error");
    return -1;
  }

  write(sockfd, response_msg, strlen(response_msg));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "RETR 1", 6)) {
    perror("RETR 1 error");
    return -1;
  }

  write(sockfd, response_120, strlen(response_120));
  // 发送1枚邮件
  char emailName[128] = "";
  if(getSendMailName(table->username, emailName) == NULL) {
    perror("no email could be send");
  } else {
    sendMail(sockfd, emailName);
    printf("成功发送邮件%s\n", emailName);
    // 删除1枚邮件
    if(unlink(emailName) == -1) {
      perror("unlink");
      return -1;
    }
  }

  write(sockfd, response_send, strlen(response_send));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "DELE 1", 6)) {
    perror("DELE 1 error");
    return -1;
  }

  write(sockfd, response_ok, strlen(response_ok));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "QUIT", 4)) {
    perror("QUIT error");
    return -1;
  }

  write(sockfd, response_ok, strlen(response_ok));
  printf("pop3连接结束\n");

  return 0;
}