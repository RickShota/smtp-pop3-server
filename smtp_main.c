/**
 * @file smtp_main.c
 * @author 黄瑞
 * @date 2024.3.30
 * @details smtp服务main源文件
 * @version 0.5
*/
#include "common.h"
#include "parsemail.h"
#include "devicectrl.h"
#include "net.h"

void *handleSMTP(void *p_cid) {
  printf("\n---SMTP子线程创建---pid = %lu\n", pthread_self());
  int cid = *((int *)p_cid);
  free(p_cid); p_cid = NULL;

  mail_t *mail = malloc(sizeof(mail_t));
  table_t table = { 0 };
  // 处理连接
  handleConnection(cid, &table, mail);
  // 解析邮件
  sub_t ctrl = { 0 };
  parseMail(mail, &ctrl);
  // 设备控制
  subjectControl(mail, &ctrl);
  // 组装邮件并写入邮件文件
  createMail(cid, mail, &ctrl);

  free(mail); mail = NULL;
  close(cid);
  printf("\n---SMTP子线程结束---pid = %lu\n", pthread_self());
  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  // 创建smtp套接字
  int smtp_sid = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in smtpAddr = { 0 };
  smtpAddr.sin_family = AF_INET;
  smtpAddr.sin_addr.s_addr = inet_addr(IP);
  smtpAddr.sin_port = htons(SMTP_PORT);

  if(bind(smtp_sid, (struct sockaddr *)&smtpAddr, sizeof(smtpAddr)) < 0) {
    perror("bind error");
    return -1;
  }

  if(listen(smtp_sid, 10) < 0) {
    perror("listen error");
    return -1;
  }
  printf("SMTP is server on %s:%d...\n", IP, SMTP_PORT);
  // 创建子进程运行pop3服务器

  while(1) {
    printf("SMTP准备连接，等待三次握手...\n");
    int cid = accept(smtp_sid, NULL, NULL);
    if(cid < 0) {
      perror("accept error");
      close(cid);
      return -1;
    }
    printf("SMTP三次握手成功! cid = %d\n", cid);
    // 创建子线程处理smtp通信
    pthread_t tid;
    // 分配新的内存空间存储cid的值
    int *p_cid = malloc(sizeof(int));
    *p_cid = cid;
    int ret = pthread_create(&tid, NULL, handleSMTP, (void *)p_cid);
    // 设置线程分离
    ret = pthread_detach(tid);
    if(ret != 0) {
      perror("pthread_detach");
      free(p_cid);
      exit(EXIT_FAILURE);
    }
  }
  close(smtp_sid);
  return 0;
}
