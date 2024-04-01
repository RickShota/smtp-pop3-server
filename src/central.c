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
  // 创建smtp和pop3套接字
  int smtp_sid, pop3_sid;
  smtp_sid = socket(AF_INET, SOCK_STREAM, 0);
  pop3_sid = socket(AF_INET, SOCK_STREAM, 0);
  if(smtp_sid < 0 || pop3_sid < 0) {
    perror("socket error");
    return -1;
  }

  // 创建各自地址结构体
  struct sockaddr_in smtpAddr = { 0 }, popAddr = { 0 };
  smtpAddr.sin_family = AF_INET;
  smtpAddr.sin_addr.s_addr = inet_addr(IP);
  smtpAddr.sin_port = htons(SMTP_PORT);

  popAddr.sin_family = AF_INET;
  popAddr.sin_addr.s_addr = inet_addr(IP);
  popAddr.sin_port = htons(POP3_PORT);

  // 套接字绑定地址
  int sb = 0, pb = 0;
  sb = bind(smtp_sid, (struct sockaddr *)&smtpAddr, sizeof(smtpAddr));
  pb = bind(pop3_sid, (struct sockaddr *)&popAddr, sizeof(popAddr));
  if(sb < 0 || pb < 0) {
    perror("bind error");
    return -1;
  }

  // 监听端口
  int sl, pl;
  sl = listen(smtp_sid, 5);
  pl = listen(pop3_sid, 10);
  if(sl < 0 || pl < 0) {
    perror("listen error");
    return -1;
  } else {
    printf("SMTP is server on %s:%d...\n", IP, SMTP_PORT);
    printf("POP3 is server on %s:%d...\n", IP, POP3_PORT);
  }

  // 创建子进程
  int main_pid = getpid();
  for(int i = 0;; i++) {
    if(i > 0 && getpid() == main_pid)
      exit(0);

    pid_t pid = fork();
    if(pid < 0) {
      perror("fork error");
      return -1;
    } else if(pid == 0) {
      mail_t *mail = malloc(sizeof(mail_t));
      table_t table = { 0 };

      int cid = accept(smtp_sid, NULL, NULL);
      if(cid < 0) {
        perror("accept error");
        close(cid);
        return -1;
      }
      handleConnection(smtp_sid, &table, mail);
      sub_t ctrl = { 0 };
      parseMail(mail, &ctrl);
      // subjectControl(mail, &ctrl);

      char fileName[128] = "";
      printf("table.username = %s\n", table.username);
      getCreatMailName(table.username, fileName);
      createMail(cid, mail, &ctrl);

      pop3Connection(pop3_sid, &ctrl, &table, mail);
      free(mail);
      mail = NULL;
      close(cid);
    } else if(pid > 0) {
      wait(NULL);
    }
  }
  // 关闭套接字
  close(smtp_sid);
  close(pop3_sid);

  return 0;
}
