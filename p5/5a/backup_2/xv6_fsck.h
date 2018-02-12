#ifndef XV6_FSCK_H
#define XV6_FSCK_H

#include <stdlib.h> 
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "fs.h"
#include "types.h"

#define SB_BN 1
#define SB_OFFSET 1*BSIZE

#define NODE_BN 2
#define NODE_OFFSET 2*BSIZE
#define NUM_ENTRIES 32
/*extern*/ struct dinode *ntable; //pointer to inode table
/*extern*/ struct superblock* sb;
/*extern*/ void * datablock;
/*extern*/ void *img_ptr;


/*
#define BMAP_BN (3 + (num_inodes/8))
#define BMAP_OFFSET (3 + (num_inodes/8))*BSIZE

#define DATABLOCK_BN (4 + (num_inodes/8)) 
#define DATABLOCK_OFFSET DATABLOCK_BN*BSIZE
#define CHAR_SIZE 8
*/

//xv6_fsck.c
void ERROR(char* );
void* getAddr(uint );
void validType(struct dinode *);
void check_inode(struct dinode *, int );


//dir.c
int 
check_parent(struct dinode *, uint);
void check_dir(struct dinode *, int );
void check_root();


//node.c

void check_blocks();
void check_file(struct dinode *, int);

#endif //_XV6FSCK_H_
