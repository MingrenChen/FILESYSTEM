#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ext2_utils.c"

unsigned char* disk;

int main(int argc, char** argv){
  init_ptrs(argv[1]);
  char *name = "/level1/level2/bfile/";
  int a = find_dir_in_dir(name, 1);
  int b = find_file_in_dir(name, a-1);
  printf("%d\n", b);
  printf("%s\n", current->name);
}
