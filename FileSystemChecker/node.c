#include "xv6_fsck.h"

void
check_blocks() {

}

void
check_file(struct dinode *ip, int inum) {
  if ( inum == 1 ) {
    ERROR("inum 1 belongs to a file");
  }
}


