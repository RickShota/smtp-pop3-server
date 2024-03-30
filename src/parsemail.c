#include "parsemail.h"

char *base64Decode(const char *bdata) {
  return NULL;
}

int verUsername(const char *username, table_t *userTable) {
  if(NULL == username || NULL == userTable)
    return -1;
  FILE *fp = fopen(USER_FILE, "r");
  if(NULL == fp) {

    perror("fopen error");
    return -1;
  }
  struct table temp = { 0 };

  while(fscanf(fp, "%s %s\n", temp.username, temp.password) != EOF) {
    if(!strncmp(username, temp.username, strlen(temp.username))) {
      memcpy(userTable, &temp, sizeof(temp));
      fclose(fp);

      return 0;
    }
  }
  return -1;
}

int verPassword(const char *password, table_t *userTable) {
  return 0;
}

int parseMail(struct mail *pmail, sub_t *subject) {
  return 0;
}