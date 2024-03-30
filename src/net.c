#include "net.h"
#include "common.h"

int handleConnection(int sockfd, table_t *table, mail_t *pmail) {
  if(sockfd < 2 || NULL == pmail)
    return -1;

  char *response220 = "220 192.168.31.58 ok\r\n";
  char *response250_HELO = "250-192.168.31.58\r\n250-PIPELINING\r\n250-SIZE 52428800\r\n250-AUTH LOGIN PLAIN\r\n250-AUTH=LOGIN\r\n250-MAILCOMPRESS\r\n250 BITMIME\r\n";
  char *response334_user = "334 VXNlcm5hbWU6\r\n";
  char *response334_pass = "334 UGFzc3dvcmQ6\r\n";

  char *response235 = "235 Authenticatin successful\r\n";
  char *response250_ok = "250 OK\r\n";
  char *response_354 = "354 End data with <CR><LF>.<CR><LF>\r\n";
  char *response_221 = "221 Bye\r\n";

  write(sockfd, response220, strlen(response220));
  char buf[1024] = "";
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
  if(getusername(sockfd, table)) {

    perror("getusername error");
    return -1;
  }
  write(sockfd, response334_pass, strlen(response334_pass));

  if(getpassword(sockfd, table)) {
    perror("getpassword error");
    return -1;
  }

  write(sockfd, response235, strlen(response235));
  if(getFromAddress(sockfd, pmail)) {
    perror("getFromAddress error");
    return -1;
  }

  write(sockfd, response250_ok, strlen(response250_ok));
  if(getToAddress(sockfd, pmail)) {
    perror("gettTpAddress error");
    return -1;
  }


  write(sockfd, response250_ok, strlen(response250_ok));
  read(sockfd, buf, sizeof(buf) - 1);
  if(strncmp(buf, "DATA", 4)) {
    perror("DATA error");
    return -1;
  }

  write(sockfd, response_354, strlen(response_354));
  if(getbody(sockfd, pmail)) {
    perror("getbody error");
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
  return 0;
}

int pop3Connection(int sockfd, sub_t *subject, table_t *p, mail_t *pmail) {
  return 0;
}