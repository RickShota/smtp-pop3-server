/**
 * @file smtp_main.c
 * @author 黄瑞
 * @date 2024.3.30
 * @brief smtp服务main源文件
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
  sub_t *ctrl = malloc(sizeof(sub_t));
  // 处理连接
  handleConnection(cid, mail);
  // 解析邮件
  parseMail(mail, ctrl);
  // 设备控制
  subjectControl(mail, ctrl);
  // 组装邮件并写入邮件文件
  createMail(cid, mail, ctrl);

  free(mail); mail = NULL;
  free(ctrl); ctrl = NULL;
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
    close(smtp_sid);
    return -1;
  }

  if(listen(smtp_sid, 10) < 0) {
    perror("listen error");
    close(smtp_sid);
    return -1;
  }
  printf("SMTP server is listening on %s:%d...\n", IP, SMTP_PORT);

  // 创建epoll实例
  int epoll_fd = epoll_create1(0);
  if(epoll_fd == -1) {
    perror("epoll_create1 error");
    close(smtp_sid);
    return -1;
  }

  struct epoll_event event, events[MAX_EVENTS];
  event.events = EPOLLIN;
  event.data.fd = smtp_sid;

  // 添加监听套接字到 epoll 实例
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, smtp_sid, &event) == -1) {
    perror("epoll_ctl error");
    close(epoll_fd);
    close(smtp_sid);
    return -1;
  }

  // 创建子进程运行pop3服务器
  while(1) {
    // 等待事件发生
    int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if(num_events == -1) {
      perror("epoll_wait error");
      break;
    }
    // 处理所有事件
    for(int i = 0; i < num_events; ++i) {
      if(events[i].data.fd == smtp_sid) {
        printf("SMTP准备连接，等待三次握手...\n");
        int cid = accept(smtp_sid, NULL, NULL);
        if(cid == -1) {
          perror("accept error");
          continue;
        }
        printf("SMTP三次握手成功! cid = %d\n", cid);

        // 将新连接的套接字添加到 epoll 实例中
        event.data.fd = cid;
        event.events = EPOLLIN | EPOLLET; // 设置边缘触发模式
        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cid, &event) == -1) {
          perror("epoll_ctl error");
          close(cid);
          continue;
        }

        // 创建线程处理 SMTP 通信
        pthread_t tid;
        int *p_cid = malloc(sizeof(int));
        *p_cid = cid;
        int ret = pthread_create(&tid, NULL, handleSMTP, (void *)p_cid);
        // 设置线程分离
        ret = pthread_detach(tid);
        if(ret != 0) {
          perror("pthread_detach error");
          free(p_cid);
          close(cid);
          continue;
        }
      }
    }
  }
  // 关闭 epoll 实例和 SMTP 套接字
  close(epoll_fd);
  close(smtp_sid);

  return 0;
}
