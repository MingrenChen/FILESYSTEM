//
// Created by LiAllen on 2018-03-19.
//
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ext2_utils.h"


unsigned char* disk;


int main(int argc, char **argv) {
    if(argc != 4) {
        fprintf(stderr, "Usage: %s <image file name> <src file path> <abs path of file sys>\n", argv[0]);
        exit(1);
    }

    /**
    TODO: check if it's absolute path
    **/
    // if (argv[1][3]!='/') {
    //     fprintf(stderr,"not abs path");
    //     exit(1);
    // }


    init_ptrs(argv[1]);

    // check if disk has enough space.
    if (sb->s_free_blocks_count == 0 || sb->s_free_inodes_count == 0){
        perror("no enough space");
        exit(ENOMEM);
    }

    // open src file and get it's Size.
    FILE* src = fopen(argv[2], "rb");
	  if (src==NULL) {
		    fprintf(stderr,"ENOENT:%s\n",strerror(ENOENT));
		    exit(1);
	  }
    fseek (src, 0, SEEK_END);
    int fileSize = ftell(src);
    fseek(src, 0, SEEK_SET);

    // find root dir
    char *name = argv[3];
    int dir_inode_num = find_dir_in_dir(name, EXT2_ROOT_INO);
    // if ((int dir_inode_num = find_dir_in_dir(name, EXT2_ROOT_INO)) > 0) {
    //   /* code */
    // }



}
