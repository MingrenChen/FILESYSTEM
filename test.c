#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char* tokenize(char *abspath){
  abspath += 1;
  int count = 0;
  size_t i = 0;
  for (i = 0; i < strlen(abspath); i++) {
    if (abspath[i] == '/') {
      count ++;
    }
    if (count == 1) {
      break;
    }
  }
  char* parent = malloc(sizeof(char*));
  strncpy(parent, abspath, i);
  return parent;
}

int main(int argc, char** argv){
  char *a = "/a/b/c/d/e";
  char *b = tokenize(a);
  printf("%s\n", b);
  a = a + strlen(b) + 1;
  printf("%s\n", a);
}
