#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "ext2_utils.c"

unsigned char* disk;

int main(int argc, char** argv){
  init_ptrs(argv[1]);
  int a = find_dir_in_dir("/level1/level2/", 1);
}
