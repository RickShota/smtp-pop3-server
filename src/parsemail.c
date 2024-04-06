/**
 * @file parsemail.h
 * @author 黄瑞
 * @date 2024.3.30
 * @brief 邮件解析模块源文件
*/
#include "parsemail.h"

// 验证用户名的合法性并存储
int verUsername(const char *username, table_t *userTable) {
  if(NULL == username || NULL == userTable)
    return -1;
  FILE *fp = fopen(USER_FILE, "r");
  if(NULL == fp) {
    perror("fopen error");
    return -1;
  }
  flock(fileno(fp), LOCK_EX); // 加锁
  table_t temp = { 0 };
  while(fscanf(fp, "%s %s\n", temp.username, temp.password) != EOF) {
    if(!strncmp(username, temp.username, strlen(temp.username))) {
      memcpy(userTable, &temp, sizeof(temp));
      flock(fileno(fp), LOCK_UN); // 解锁
      fclose(fp);
      return 0;
    }
  }
  flock(fileno(fp), LOCK_UN); // 解锁
  fclose(fp);
  return -1;
}

// 验证密码的合法性并存储
int verPassword(const char *password, table_t *userTable) {
  if(NULL == password || NULL == userTable)
    return -1;
  if(!strcmp(password, userTable->password))
    return 0;
  return -1;
}

// 解析邮件
int parseMail(struct mail *pmail, sub_t *subject) {
  if(NULL == pmail || NULL == subject)
    return -1;
  printf("开始解析邮件...\n");
  const char *sub = "Subject:"; // 起始标记字符串
  char *start = strstr(pmail->raw, sub) + 9; // 定位到起始位置之后
  if(NULL == start)
    return -1;
  char *end = strstr(start, "\r\n"); // 查找结束位置
  if(NULL == end)
    return -1;
  int len = end - start + 1; // 计算主题内容的长度
  char buf[128] = "";
  strncpy(buf, start, len);
  const char *command = strtok(buf, " "); // 使用空格作为分隔符,截取命令
  if(NULL == command)
    return -1;
  strcpy(subject->command, command);
  // 根据命令部分的不同进行不同的处理
  if(!strcmp(command, "8LED")) {
    const char *bulb = strtok(NULL, " ");
    subject->bulb = atoi(bulb);
    const char *bulb_ctrl = strtok(NULL, " ");
    subject->bulb_ctl = atoi(bulb_ctrl);
  } else if(!strcmp(command, "7SHU")) {
    const char *num = strtok(NULL, " ");
    subject->signal = atoi(num);
  } else if(!strcmp(command, "MOTO")) {
    const char *reva = strtok(NULL, " ");
    subject->reva = atoi(reva);
  } else if(!strncmp(command, "CHANGETABLE", 11)) {
    printf("table.txt更新成功\n");
  } else
    return -1;
  printf("解析成功!\n\n");
  return 0;
}