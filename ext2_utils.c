//
// Created by LiAllen on 2018-03-19.
//
#include <string.h>
#include "ext2_utils.h"


/*
 * This function is for initialize all necessary pointer for every operations.
 * On success return image file descriptor, o.w. exit.
 */
int init_ptrs(char* img_file){
    int fd = open(img_file, O_RDWR);

    if (fd < 0){
        perror("Error opening file.");
        exit(ENOENT);
    }

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
    gdt = (struct ext2_group_desc *)(disk + EXT2_BLOCK_SIZE * 2);
    block_bm = disk + (EXT2_BLOCK_SIZE * gdt->bg_block_bitmap);
    inode_bm = disk + (EXT2_BLOCK_SIZE * gdt->bg_inode_bitmap);
    inode_table = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * gdt->bg_inode_table);

    return fd;
}
/*
 * This function is used for finding a free inode in inode bitmap.
 * If there is no free inode now, return -1 indicating error.
 */
int find_free_inode(){
    int idx = 0;
    for (int byte = 0; byte < sb->s_inodes_count / 8; byte++){
        for (int bit = 0; bit < 8; bit++){
            if (inode_bm[byte] & (1 << bit)){
                idx++;
            } else{
                return idx;
            }
        }
    }
    return -1;
}

/*
 * This function is used for finding a free block in block bitmap.
 * If there is no free block now, return -1 indicating error.
 */
int find_free_block(){
    int idx = 0;
    for (int byte = 0; byte < sb->s_blocks_count / 8; byte++){
        for (int bit = 0; bit < 8; bit++){
            if (block_bm[byte] & (1 << bit)){
                idx++;
            } else{
                return idx;
            }
        }
    }
    return -1;
}

/*
 * This function is used to set bits of inode/block bitmap on or off.
 * bm_idx can be 0 or 1 indicates inode bitmap or block bitmap
 * idx indicates bit index in bitmap. if mode is 0, turn idx on, o.w. off.
 */
void set_bitmap(int bm_idx ,int idx, int mode){
    int num_bit;
    if (!bm_idx){
        num_bit = sb->s_blocks_count;
    } else{
        num_bit = sb->s_inodes_count;
    }
    int cur_idx = 0;

    for (int byte = 0; byte < num_bit / 8; byte++){
        for (int bit = 0; bit < 8; bit++){
            if (cur_idx == idx){
                if (mode){
                    block_bm[byte] = (unsigned char) (block_bm[byte] | (1 << bit));
                } else{
                    int temp = 0xff;
                    temp = temp ^ (1 << bit);
                    block_bm[byte] = (unsigned char) (block_bm[byte] & temp);
                }
            }
        }
    }
}

int find_dir_by_name(char *name[]){
  return 0;
}


size_t count(const char* s, char c) {
    size_t r = 0;
    for(; *s; s++)
        r += *s == c;
    return r;
}

// this function split the outer directory from a std abspath.
// for example /root/b/folder2/untitled/ should return root.
// to get the remaining, use origin = origin + strlen(result) + 1
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


char* standardize(char *name){
  char* new_name = malloc(sizeof(char*));
  int flag = 0;
  int i;
  for (i = 0; i < strlen(name); i++) {
    if (name[i] != '/') {
      if (flag == 1) {
        flag = 0;
      }
      strncat(new_name, &name[i], 1);
    } else if (flag == 0){
      flag = 1;
      strncat(new_name, &name[i], 1);
    }
  }
  if (new_name[i-1] != '/') {
    strncat(new_name, "/", 1);
  }
  return new_name;
}

// find a file in a directory.
char* findFileName(char *name){
  char *fileName = malloc(sizeof(char*));
  int k = 1 + strlen(name) - strlen(strchr(name + 1, '/'));
  strncpy(fileName, name+k, strlen(name)-k-1);
  return fileName;
}

// get the name of file of a dir entry.
char *getName(struct ext2_dir_entry* entry){
  char *name = malloc(sizeof(char*));
  strncpy(name, entry->name, entry->name_len);
  return name;
}

/* This function can find if a dir is in */
int find_dir_in_dir(char *name, int inode_num){
    char *dirName = tokenize(name);
    name = name + strlen(dirName) + 1;
    unsigned int endAddress = 0;
    // file's parent directory
    if (count(name, '/') == 3){
      struct ext2_dir_entry * current = (struct ext2_dir_entry *)\
      (disk + inode_table[inode_num].i_block[0] * EXT2_BLOCK_SIZE);
      endAddress = endAddress + current->rec_len;
      while (endAddress != 1024){
        char *entry_name = getName(current);
        if (strcmp(entry_name, dirName) == 0 && current->file_type == EXT2_FT_DIR) {
          free(dirName);
          free(entry_name);
          return current->inode;
        }
        free(entry_name);
      }
    } else { // not the parent directory
      struct ext2_dir_entry * current = (struct ext2_dir_entry *)\
      (disk + inode_table[inode_num].i_block[0] * EXT2_BLOCK_SIZE);
      endAddress += current->rec_len;
      while (endAddress != 1024){
        char *entry_name = getName(current);
        if (strcmp(entry_name, dirName) == 0 &&
         current->file_type == EXT2_FT_DIR) {
          free(dirName);
          free(entry_name);
          return find_dir_in_dir(name, current->inode);
        }
        free(entry_name);
      }
    }
    free(dirName);
    return -1;
}

int find_file_in_dir(char *name, int inode_num){
  char *fileName = findFileName(name);
  struct ext2_dir_entry * current = (struct ext2_dir_entry *)\
  (disk + inode_table[inode_num].i_block[0] * EXT2_BLOCK_SIZE);
  unsigned int endAddress = 0;
  while (endAddress != 1024){
    endAddress += current->rec_len;
    char *entry_name = getName(current);
    if (strcmp(entry_name, fileName) == 0 &&
     current->file_type == EXT2_FT_REG_FILE) {
       free(fileName);
      return current->inode;
    }
    current = (struct ext2_dir_entry *)\
    (disk + inode_table[inode_num].i_block[0] * EXT2_BLOCK_SIZE + endAddress);
  }
  free(fileName);
  return -1;
}
