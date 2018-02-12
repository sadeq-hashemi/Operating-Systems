#include "xv6_fsck.h"

/*
void 
check_bitmap()
void
check_blocks() {
}
*/
short 
directory_references(int inum) {
  short dir_reference = 0;

  struct dinode *i;
  uint ind;  
  uint* addr, *indirect_addr;
  struct dirent *j, *direntry;
  

  for(i = ntable; i < &ntable[sb->ninodes]; i ++) {
     if(i->type == T_DIR) {
       for(ind = 0; ind <NDIRECT; ind++) {
          addr = &(i->addrs[ind]);
           gcc xv6_fsck.c file.c dir.c -o xv6_fsck -Wall -Werror          
          for(j = (struct dirent *)addr; j < &(struct dirent *)addr[32] ; j ++) {
           if(j->inum  == inum){
             dir_reference++;
           }
          }
       }

       addr  = &(i->addrs[13]);        
       for(ind = 0; ind < NINDIRECT; ind++) {
          indirect_addr = &addr[ind];
             for(j = (struct dirent *)indirect_addr; j < &(struct dirent *)addr[32] ; j ++) {
                if(j->inum  == inum){
                   dir_reference++;
                }
             }
       }
    
     }
  }
   
  return dir_reference;
}



void
check_file(struct dinode *ip, int inum) {

  short file_ref_count;
  short dir_ref; 
 
  if ( inum == 1 ) {
    ERROR("inum 1 belongs to a file");
  } 

  else if (ip->type == T_FILE) {
     file_ref_count = ip->nlink;
     dir_ref = directory_references(inum); 
     
     if(file_ref_count != dir_ref){
        ERROR("bad reference count for file");
     }
  }
}


