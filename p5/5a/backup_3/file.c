#include "xv6_fsck.h"

void inode_inuse(struct dinode *ip, int inum) {

  struct dinode *i;
  uint ind;
  uint * indirect_block_addr;
  struct dirent *j, *addr, *indirect_addr;
  int references;

  if(ip->type != 0) {
      for(i = ntable; i < &ntable[sb->ninodes]; i++) {
         if(i->type == T_DIR) {
            for(ind = 0; ind <NDIRECT; ind++) {
              addr = (struct dirent *)getAddr(i->addrs[ind]);
               if(addr == 0)  {
                 break;
               }
                for(j = addr; j < &addr[NUM_ENTRIES]; j ++) {
                   if(j->inum  == inum){
                        references++;
                   } 
                }
             } 

             indirect_block_addr  = (uint *)getAddr(i->addrs[NDIRECT + 1]);
               if(indirect_block_addr == 0)  {
                  break;
               }
              for(ind = 0; ind < NINDIRECT; ind++) {
                 indirect_addr = (struct dirent *)getAddr(indirect_block_addr[ind]);
                    if(indirect_addr == 0)  {
                       break;
                     }
                    for(j = indirect_addr; j < &indirect_addr[NUM_ENTRIES] ; j++) {
                       if(j->inum  == inum){
                           references++;
                        }
                    }  
               }    
         }
      }
   }
   
   if(references == 0) {
     ERROR("inode marked use but not found in a directory");
   }
}

short 
directory_references(int inum) {
  short dir_reference = 0;

  struct dinode *i;
  uint ind; 
  uint *indirect_block_addr; 
  struct dirent *j, *addr, *indirect_addr;
  

  for(i = ntable; i < &ntable[sb->ninodes]; i ++) {
     if(i->type == T_DIR) {
       for(ind = 0; ind <NDIRECT; ind++) { 
           addr = (struct dirent *)getAddr(i->addrs[ind]);
           if(addr == 0)  {
             break;
           }
          for(j = addr; j < &addr[NUM_ENTRIES]; j ++) {
           if(j->inum  == inum){
             dir_reference++;
           }
          }
       }
       
       indirect_block_addr  = (uint *) getAddr(i->addrs[NDIRECT + 1]);
        if(addr == 0)  {
          break;
       }        
       for(ind = 0; ind < NINDIRECT; ind++) {
          indirect_addr = (struct dirent *)getAddr(indirect_block_addr[ind]);
           if(indirect_addr == 0)  {
              break;
           }
             for(j = indirect_addr; j < &indirect_addr[NUM_ENTRIES] ; j++) {
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


