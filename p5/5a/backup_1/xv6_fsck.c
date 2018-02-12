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

#define SB_OFFSET 1*BSIZE 
#define NODE_OFFSET 2*BSIZE
#define BMAP_OFFSET 28*BSIZE
#define DATABLOCK_OFFSET 29*BSIZE
#define CHAR_SIZE 8

int root; //presence of root dir
struct dinode *ntable; //pointer to inode table
struct superblock* sb;
void * datablock;
uchar *bmp; //pointer to block bitmap table

void
ERROR(char* err) {
  fprintf(stderr, "ERROR: %s\n", err);
  exit(1);
}

void*
getAddr(uint bn) {
return datablock + (bn*BSIZE);
}
void
validType(struct dinode *ip) {
  if(ip->type < 0 || ip->type > 3){
    ERROR("bad inode");
  }
}

//looks through direntries and finds arg inum
int 
check_parent(struct dinode *iparent, uint child_inum) {
  uint *addrs;
  uint *indaddrs;
  struct dirent* entry;
  addrs = (uint*)iparent->addrs;
  for(int i= 0; i < NDIRECT; i++) {
    if(addrs[i] == 0)
      return -1;
    entry = (struct dirent*) getAddr(addrs[i]);
    if(entry->inum == child_inum)
      return 0;
  }

  if(addrs[NDIRECT + 1] == 0)
    return -1;
  indaddrs = (uint*) getAddr(addrs[NDIRECT + 1]);
  
  for(int i = 0; i < NINDIRECT; i++) {
    if(indaddrs[i] == 0)
      return -1;

    entry = (struct dirent*) getAddr(indaddrs[i]);
    if(entry->inum == child_inum)
      return 0;  
  }
  return -1;
}

void
check_blocks() {

}

void
check_root() {

}

void
check_file(struct dinode *ip, int inum) {
  if ( inum == 1 ) {
    ERROR("inum 1 belongs to a file");
  }
}

void
check_dir(struct dinode *ip, int inum) {
 int ind, found;
 uint* indaddr;
 uint* addrs, *addrs_temp;
 uint inum_entry;
 void* data;
 struct dirent* self, *parent, *dirent;
 struct dinode* temp;
  addrs = ip->addrs;
  if(addrs[0] == 0 || addrs[1] == 0)
    ERROR("directory not properly formatted");

  self = (struct dirent*) getAddr(addrs[0]); 
  parent = (struct dirent*) getAddr(addrs[1]);

  if((self->inum != inum) || (parent->inum <= 0) )
    ERROR("directory not properly formatted");  
  if ( (strcmp((self->name), ".") != 0))
    ERROR("directory not properly formatted");
  if ( (strcmp((parent->name), "..") != 0))
    ERROR("directory not properly formatted");

  //parent case
  inum_entry = parent->inum;
  temp = &ntable[inum_entry];
  if(temp->type == 0 || temp->type > 3 )
    ERROR("parent directory mismatch");

  addrs_temp = temp->addrs;
  //rest of entries
  ind = 2; //start with parent
  while(addrs[ind] != 0 && ind < NDIRECT) {
  dirent = (struct dirent*) getAddr(addrs[ind]);
  inum_entry = dirent->inum;  
  ind++;
  }
 //if inum 1 is not root dir root = 0;
  if ( inum == 1 ) {
      
  }

}

void
check_inode(struct dinode *ip, int inum) {
 int type;
  if(ip == NULL) {
    ERROR("passed invalid ip to check_inode()");
  }
  type = ip->type;
  switch (type) {
    case T_DIR:
       check_dir(ip, inum);
       break;
    case T_FILE:
       check_file(ip, inum);
       break;
    default : break;
  }
}
 
int
main (int argc, char* argv[]) {
int ret, fd, ind; 
void *img_ptr;
struct stat buf;
struct dinode *dip, *i;
struct direct *dir;

  root = 0;
  fd = open("fs.img", O_RDWR);
  if ( fd < 0 ) {
    ERROR("file not found");
  }
  if ( fstat(fd, &buf) < 0 ) {
    ERROR("fstat failed");
  }
  img_ptr = (void*) mmap(NULL, buf.st_size,
            PROT_READ, MAP_PRIVATE, fd, 0);
  if ( img_ptr < 0 ) {
    ERROR("mmap failed");
  }
  ntable = dip = (struct dinode*)(img_ptr + NODE_OFFSET);
  bmp = (uchar*)(img_ptr + BMAP_OFFSET);
  sb = (struct superblock*) (img_ptr + SB_OFFSET);
  datablock = img_ptr + DATABLOCK_OFFSET;
  for(i = ntable; i<&ntable[sb->ninodes]; i++) {
    ind = (int) (i - ntable);
    validType(i);
  }
  if(!root)
    ERROR("root directory does not exist");
return 0;
}
