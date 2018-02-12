#include "xv6_fsck.h"

int root; //presence of root dir
struct dinode *ntable; // ALSO DEFINED IN xv6_fsck.h. REMOVE THIS LATER
struct superblock* sb;
void * datablock;
void *img_ptr;
uchar *bmp; //pointer to block bitmap table

void
ERROR(char* err) {
  fprintf(stderr, "ERROR: %s\n", err);
  exit(1);
}

void*
getAddr(uint bn) {
  if(bn < DATABLOCK_BN)
    return 0;
  return img_ptr + (bn*BSIZE);
}

void
validType(struct dinode *ip) {
  if(ip->type < 0 || ip->type > 3){
    ERROR("bad inode");
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
int fd, ind; 
struct stat buf;
struct dinode *dip, *i;

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
    //ind = (int) (i - ntable);
    validType(i);
  }
  //FIGURE HOW ROOT IS PASSED, ask about extern
  if(!root)
    ERROR("root directory does not exist");
return 0;
}
