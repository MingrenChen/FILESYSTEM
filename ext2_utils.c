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
    int num_bit;<string.h>
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
  char* parent = malloc(sizeof(char*));size_t count(const char* s, char c) {
    size_t r = 0;
    for(; *s; s++)
        r += *s == c;
    return r;
}
  strncpy(parent, abspath, i);
  return parent;
}

/* This function can find if a dir is in */
int find_dir_in_dir(char *name, int inode_num){
    // struct ext2_inode *c_inode = (struct ext2_inode *)((disk + EXT2_BLOCK_SIZE * gdt->bg_inode_table));
    // this is the
    if (count(name, '/') == 2){

    }
    return 0;
}
