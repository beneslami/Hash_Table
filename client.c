#include <stdio.h>
#include <string.h>

extern int create_and_write_shared_memory(char *mmap_key, char *value, unsigned int size);

int main(int argc, char **argv){
  char *key = "/test2";
  char content[64];
  int select, i;

  printf("Select one of the items:\n");
  printf("1) add item\n");
  printf("2) delete item\n");
  printf("3) show table\n");
  scanf("%d", &select);

  if(select == 1){
    char temp[32];
    printf("enter your data item to add\n");
    scanf("%s", temp);
    sprintf(content, "%d%s", select, temp);
    create_and_write_shared_memory(key, content, strlen(content));
  }

  else if (select == 2){
    char temp[32];
    printf("enter your data item to delete\n");
    scanf("%s", temp);
    sprintf(content, "%d%s", select, temp);
    create_and_write_shared_memory(key, content, strlen(content));
  }
  else if (select == 3){
    sprintf(content, "%d", select);
  }
  return 0;
}
