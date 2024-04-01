/**
 * @file central.c
 * @author 黄瑞
 * @date 2024.3.30
 * @details 入口程序源文件
 * @version 0.5
*/
#include "common.h"
#include "parsemail.h"
#include "devicectrl.h"
#include "net.h"

int main(int argc, char const *argv[]) {
  // 创建smtp套接字
  int smtp_sid = socket(AF_INET, SOCK_STREAM, 0);
  // int pop3_sid = socket(AF_INET, SOCK_STREAM, 0);
  if(smtp_sid < 0) {
    perror("socket error");
    return -1;
  }

  // 创建各自地址结构体
  struct sockaddr_in smtpAddr = { 0 };
  smtpAddr.sin_family = AF_INET;
  smtpAddr.sin_addr.s_addr = inet_addr(IP);
  smtpAddr.sin_port = htons(SMTP_PORT);

  // struct sockaddr_in popAddr = { 0 };
  // popAddr.sin_family = AF_INET;
  // popAddr.sin_addr.s_addr = inet_addr(IP);
  // popAddr.sin_port = htons(POP3_PORT);

  // 套接字绑定地址
  int sb = bind(smtp_sid, (struct sockaddr *)&smtpAddr, sizeof(smtpAddr));
  // int pb = bind(pop3_sid, (struct sockaddr *)&popAddr, sizeof(popAddr));
  if(sb < 0) {
    perror("bind error");
    return -1;
  }

  // 监听端口
  int sl = listen(smtp_sid, 10);
  // int pl = listen(pop3_sid, 10);
  if(sl < 0) {
    perror("listen error");
    return -1;
  } else {
    printf("SMTP is server on %s:%d...\n", IP, SMTP_PORT);
    // printf("POP3 is server on %s:%d...\n", IP, POP3_PORT);
  }

  // 创建子进程
  for(int i = 0;; i++) {
    pid_t pid = fork();
    if(pid < 0) {
      perror("fork error");
      return -1;
    } else if(pid == 0) {
      printf("\n子进程创建成功, pid = %d\n", getpid());
      mail_t *mail = malloc(sizeof(mail_t));
      table_t table = { 0 };
      // 接受连接
      printf("SMTP准备连接，等待三次握手...\n");
      int cid = accept(smtp_sid, NULL, NULL);
      if(cid < 0) {
        perror("accept error");
        close(cid);
        return -1;
      }
      printf("三次握手成功!\n");
      handleConnection(cid, &table, mail);
      printf("SMTP连接处理结束\n");
      sub_t ctrl = { 0 };
      // 解析邮件
      parseMail(mail, &ctrl);
      // 设备控制
      // subjectControl(mail, &ctrl);
      char fileName[128] = "";
      // 获取新建邮件名
      getCreatMailName(table.username, fileName);
      // 组装邮件并写入邮件文件
      createMail(cid, mail, &ctrl);

      // pop3锁步
      // pop3Connection(pop3_sid, &ctrl, &table, mail);
      free(mail);
      mail = NULL;
      close(cid);
    } else if(pid > 0) {
      wait(NULL);
    }
  }
  // 关闭套接字
  close(smtp_sid);
  // close(pop3_sid);

  return 0;
}
