/**
 * @file deviceponse.c
 * @author 黄瑞
 * @date 2024.4.3
 * @details 控制响应模块源文件
*/
#include "devicereponse.h"

int sendMail(int sockfd, const char *mailName) {
  FILE *fp = fopen(mailName, "r");
  if(NULL == fp) {
    perror("sendMail: open error");
    return -1;
  }
  // TODO 更新时间
  char buf[MAX_MAIL];
  if(fread(buf, 1, sizeof(buf) - 1, fp) < 0)
    return -1;
  write(sockfd, buf, strlen(buf));

  fclose(fp);
  return 0;
}