/**
 * @file devicectrl.c
 * @author 黄瑞
 * @date 2024.4.1
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
  char command[32];
  strcpy(command, subject->command);
  if(strncmp(command, "8LED", 4) || strncmp(command, "7SHU", 4) || strncmp(command, "Moto", 4)) {
    emitCommand(subject);
  }
  char str[] = "changeTable";
  if(!strncasecmp(command, str, strlen(str))) {
    emitUpdate(pmail, subject);
    printf("subject->command: %s\n", subject->command);

    printf("subject->bulb: %d\n", subject->bulb);
    printf("subject->bulb_ctl: %d\n", subject->bulb_ctl);
    return 0;
  }

  strcpy(subject->command, command);
  printf("subject->command: %s\n", subject->command);
  printf("subject->bulb: %d\n", subject->bulb);
  printf("subject->bulb_ctl: %d\n", subject->bulb_ctl);

  return -1;
}

// 执行更新操作，将邮件附件内容写入"table.txt"文件中
int emitUpdate(const mail_t *pmail, sub_t *subject) {
  char command[32] = "";
  strcpy(command, subject->command);
  if(strncpy(command, "changTable", 11)) {
    FILE *fd = fopen("table.txt", "w");

    char attr[1024 * 10] = "";
    strcpy(attr, pmail->attr);
    if(fwrite(attr, sizeof(char), strlen(attr), fd) < strlen(attr)) {
      perror("fwrite error");
      fclose(fd);
      return -1;
    }
    fclose(fd);
    printf("attr: %s\n", attr);
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
    printf("%d 灯泡状态为 %d\n", subject->bulb, subject->bulb_ctl);
    return 0;
  } else if(strcmp(command, "7SHU") == 0) {
    if(subject->signal < 0 || subject->signal > 99) {
      subject->result = -1;
      printf("调用七段数码管控制函数失败\n");
      return -1;
    }
    subject->result = 0;

    printf("七段数码管控制成功\n");
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
  } else if(!strncmp(command, "CHANGTABLE", 10)) {
    printf("table.txt更新成功\n");
    return 0;
  } else {
    printf("识别不了命令字控制\n");
    return -2;
  }
}

// 生成一个新的邮件文件名
char *getCreatMailName(const char *userName, char *mailName) {
  char buf[128];
  for(int i = 1;; i++) {
    char num[10];
    sprintf(num, "%d", i);
    memset(buf, 0, sizeof(buf));
    strcpy(buf, userName);

    strcat(buf, "_");
    strcat(buf, num);
    strcat(buf, ".mail");
    if(access(buf, F_OK) == -1) {
      strcpy(mailName, buf);
      return NULL;
    }
  }
  return NULL;
}

// 查找邮件文件名中尚未被使用的编号
char *getSendMailName(const char *userName, char *mailName) {
  char buf[128];
  for(int i = 0; i < 100; i++) {
    char num[10];
    sprintf(num, "%d", i);
    memset(buf, 0, sizeof(buf));
    strcpy(buf, userName);
    strcat(buf, "_");
    strcat(buf, num);
    if(!access(buf, F_OK)) {
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
  time_t raw_time;
  struct tm *time_info;
  char *time_str = malloc(128 * sizeof(char));
  time(&raw_time);
  time_info = localtime(&raw_time);
  strftime(time_str, 128, "%c", time_info);
  return time_str;
}