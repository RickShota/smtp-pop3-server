/**
 * @file devicectrl.c
 * @author 黄瑞
 * @date 2024.4.2
 * @details 设备控制模块源文件
*/
#include "devicectrl.h"

char *getName(const char *email) {
  char *name;
  char *atSymbol = strchr(email, '@');
  if(atSymbol == NULL)
    name = strdup(email);
  else {
    size_t len = atSymbol - email;
    name = malloc((len + 1) * sizeof(char));
    strncpy(name, email, len);
    name[len] = '\0';
  }
  return name;
}

char *readFile(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if(file == NULL) {
    perror("File open failed");
    exit(EXIT_FAILURE);
  }
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *buf = (char *)malloc(fileSize + 1);
  if(buf == NULL) {
    perror("Memory allocation failed");
    fclose(file);
    exit(EXIT_FAILURE);
  }
  size_t bytesRead = fread(buf, 1, fileSize, file);
  if(bytesRead != fileSize) {
    perror("File read failed");
    free(buf);
    fclose(file);
    exit(EXIT_FAILURE);
  }
  buf[fileSize] = '\0';
  fclose(file);
  return buf;
}

// 处理邮件对象和主题控制结构
int subjectControl(const mail_t *pmail, sub_t *subject) {
  printf("尝试控制设备...\n\n");
  char command[32];
  strcpy(command, subject->command);
  printf("命令行: %s\n", subject->command);
  printf("灯号数: %d\n", subject->bulb);
  printf("控制信号: %d\n\n", subject->bulb_ctl);
  if(!strncasecmp(command, "CHANGTABLE", 11)) {
    emitUpdate(pmail, subject);
    return 0;
  } else if(strncmp(command, "8LED", 4) || strncmp(command, "7SHU", 4) || strncmp(command, "Moto", 4)) {
    emitCommand(subject);
    return 0;
  }
  return -1;
}

// 执行更新操作，将邮件附件内容写入"table.txt"文件中
int emitUpdate(const mail_t *pmail, sub_t *subject) {
  char command[32] = "";
  strcpy(command, subject->command);

  if(strncpy(command, "CHANGTABLE", 11)) {
    int fd = open(USER_FILE, O_WRONLY);
    if(fd == -1) {
      perror("open");
      return -1;
    }
    char attr[1024 * 10] = "";
    strcpy(attr, pmail->attr);
    // 加锁
    flock(fd, LOCK_EX);
    if(write(fd, attr, strlen(attr)) < strlen(attr)) {
      perror("fwrite error");
      flock(fd, LOCK_UN); // 解锁
      close(fd);
      return -1;
    }
    printf("table.txt更新成功：\n%s\n", attr);
    flock(fd, LOCK_UN); // 解锁
    close(fd);
    return 0;
  }
  return -1;
}

// 根据主题控制结构中的命令执行相应的控制操作
int emitCommand(sub_t *subject) {
  char command[32] = "";
  strcpy(command, subject->command);
  if(strcmp(command, "8LED") == 0) {
    if(subject->bulb < 1 || subject->bulb > 8) {
      subject->result = -1;
      printf("灯泡的号数有误:%d, 应为1~8\n", subject->bulb);
      return -1;
    }
    if(subject->bulb_ctl != 0 && subject->bulb_ctl != 1) {
      subject->result = -1;
      printf("灯泡控制信号有误:%d, 应为0或1\n", subject->bulb);
      return -1;
    }
    subject->result = 0;
    printf("\n%d号灯泡状态为 %d\n\n", subject->bulb, subject->bulb_ctl);
    return 0;
  } else if(strcmp(command, "7SHU") == 0) {
    if(subject->signal < 0 || subject->signal > 99) {
      subject->result = -1;
      printf("调用七段数码管控制函数失败,信号不合法\n");
      return -1;
    }
    subject->result = 0;
    printf("七段数码管控制成功!\n");
    return 0;
  } else if(strcmp(command, "MOTO") == 0) {
    if(subject->reva < 0 || subject->reva > 999) {
      subject->result = -1;
      printf("调用电动机控制失败\n");
      return -1;
    }
    subject->result = 0;
    printf("电动机控制成功\n");
    return 0;
  } else {
    printf("识别不了的指令: %s\n", command);
    return -1;
  }
}

// 生成一个新的邮件文件名
char *getCreatMailName(const char *userName, char *mailName) {
  printf("开始生成新的邮件文件名...\n");
  char buf[128];
  for(int i = 1;; i++) {
    char num[10];
    sprintf(num, "%d", i);
    memset(buf, 0, sizeof(buf));

    strcpy(buf, "database/");
    strcat(buf, userName);
    strcat(buf, "_");
    strcat(buf, num);
    strcat(buf, ".mail");
    // 检查文件是否重名
    if(access(buf, F_OK) == -1) {
      strcpy(mailName, buf);
      printf("生成成功，邮件文件名为：%s\n", mailName);
      return mailName;
    }
  }
  return NULL;
}

// 获取要发送的邮件文件名
char *getSendMailName(const char *userName, char *mailName) {
  char buf[128];
  for(int i = 0; i < 100; i++) {
    char num[10];
    sprintf(num, "%d", i);
    memset(buf, 0, sizeof(buf));

    strcpy(buf, "database/");
    strcat(buf, userName);
    strcat(buf, "_");
    strcat(buf, num);
    strcat(buf, ".mail");
    // 检查文件是否存在
    if(access(buf, F_OK) == 0) {
      strcpy(mailName, buf);
      return mailName;
    }
  }
  return NULL;
}

// 创建邮件文件并发送邮件内容到指定的套接字
int createMail(int sockfd, const mail_t *pmail, sub_t *subject) {
  char filename[64];
  char *sendName = getName(pmail->send);
  char *recvName = getName(pmail->recv);
  getCreatMailName(sendName, filename);

  char *time = getCurtime();
  printf("%s\n", time);
  FILE *file = fopen(filename, "w");

  fprintf(file, "Message-ID: <%s>\\r\\n\r\n", "001101c93cc4$1ed2ae30$5400a8c0@liuxiaoforever");
  fprintf(file, "From: \"%s\" <%s>\\r\\n\r\n", sendName, pmail->send);
  fprintf(file, "To: %s<%s>\\r\\n\r\n", recvName, pmail->recv);
  fprintf(file, "Subject:%s %d %d Control %s! \\r\\n\r\n", subject->command, subject->bulb, subject->signal, subject->result == 0 ? "Success" : "Fail");

  fprintf(file, "Date: %s\\r\\n\r\n", time);
  fprintf(file, "MIME-Version: 1.0\\r\\n\r\n");
  fprintf(file, "Content-Type: multipart/alternative; \\r\\n\r\n");
  fprintf(file, "X-Priority: 3..X-MSMail-Priority: Normal\\r\\n\r\n");

  fprintf(file, "X-Mailer: Microsoft Outlook Express 6.00.2900.3138\\r\\n\r\n");
  fprintf(file, "X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2900.3198\\r\\n\r\n");
  fprintf(file, "\\r\\n\r\n");
  fprintf(file, "This is response of %s's device!\\r\\n\r\n", pmail->recv);

  printf("------------------\n");
  fclose(file);

  char *fileContent = readFile(filename);
  send(sockfd, fileContent, strlen(fileContent), 0);
  free(fileContent);
  return 0;
}

// 获取当前本地时间，并格式化为字符串
char *getCurtime() {
  time_t raw_time; // 存储时间的原始数据
  struct tm *time_info; // 存储时间的结构体指针
  char *time_str = malloc(128 * sizeof(char)); // 存储时间字符串
  time(&raw_time); // 获取当前时间的原始数据
  time_info = localtime(&raw_time); // 将原始时间数据转换为本地时间
  strftime(time_str, 128, "%c", time_info); // 格式化时间为字符串并存储
  return time_str; // 返回时间字符串指针
}