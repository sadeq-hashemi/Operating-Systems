#include "xv6_fsck.h"


void bitmap_check() {

 struct dinode *inode;
 uint * indirect_block_addr;
 uint index, offset;  
 uint present;

 for(uint i = 0; i < sb->size; i++) {
   if (i < sb->size - sb->nblocks ) 
     continue; 
   index = i/((sizeof(char)) * 8 );

   offset = (i % (sizeof(char)*8));

//printf("i= %u\n index:%u offset:%x\n bmap:%x and %x -----\n", i, index, offset, bmap[index], 1<<offset); 
   present = 0;
  if( (bmap[index] & 1<<offset)  != 0) {
      for(inode = ntable; inode < &ntable[sb->ninodes]; inode++) {       
          if(inode->type != 0) { 
            for(int ind = 0; ind < NDIRECT; ind++) {
               if(inode->addrs[ind] == 0) {
                 break;
               }
               if(inode->addrs[ind] == i) {
                  present++;
               }            
            }
            if(inode->addrs[NDIRECT] == 0)
                continue;
            if(inode->addrs[NDIRECT] == i)
              present++;
            indirect_block_addr = (uint *)getAddr(inode->addrs[NDIRECT]);
        
            if(indirect_block_addr == NULL) {
                 ERROR("bad indirect address in inode");
            }
            for(int ind = 0; ind < NINDIRECT; ind++) {
               if(indirect_block_addr[ind] == 0)
                 break;
               if(indirect_block_addr[ind] == i)
                 present++;
            }

          } 
       }
  if(present < 1) {
    ERROR("bitmap marks block in use but it is not in use");  
   } 
  if(present > 1) {
    ERROR("direct address used more than once");
  }
  } else {
       for(inode = ntable; inode < &ntable[sb->ninodes]; inode++) {       
          if(inode->type != 0) { 
            for(int ind = 0; ind < NDIRECT; ind++) {
               if(inode->addrs[ind] == 0) {
                 break;
               }
               if(inode->addrs[ind] == i) {
                  present++;
               }            
            }
            if(inode->addrs[NDIRECT] == 0)
                continue;
            if(inode->addrs[NDIRECT] == i)
              present++;
            indirect_block_addr = (uint *)getAddr(inode->addrs[NDIRECT]);
        
            if(indirect_block_addr == NULL) {
                 ERROR("bad indirect address in inode");
            }
            for(int ind = 0; ind < NINDIRECT; ind++) {
               if(indirect_block_addr[ind] == 0)
                 break;
               if(indirect_block_addr[ind] == i)
                 present++;
            }

          } 
       }
  if(present > 0) {
    ERROR("address used by inode but marked free in bitmap");  
 
  } 
   
  }
}
}


void inode_inuse(struct dinode *ip, int inum) {

  struct dinode *i;
  uint ind;
  uint * indirect_block_addr;
  struct dirent *j, *addr, *indirect_addr;
  int references = 0;

  if(ip->type != 0) {
      for(i = ntable; i < &ntable[sb->ninodes]; i++) {
         if(i->type == T_DIR) {
            for(ind = 0; ind <NDIRECT; ind++) {
              if(i->addrs[ind] == 0)
                 break;
              addr = (struct dirent *)getAddr(i->addrs[ind]);
               if(addr == NULL)  {
                 ERROR("bad direct address in inode");
               }
                for(j = addr; j < &addr[NUM_ENTRIES]; j ++) {
                   if(j->inum  == inum){
                        references++;
                   } 
                }
             } 
         
             if(i->addrs[NDIRECT] != 0) {
             indirect_block_addr  = (uint *)getAddr(i->addrs[NDIRECT]);
               if(indirect_block_addr == NULL)  {
                  ERROR("bad indirect address in inode");
               }
              for(ind = 0; ind < NINDIRECT; ind++) {
                 if(indirect_block_addr[ind] == 0){
                     break;
                 }
                 indirect_addr = (struct dirent *)getAddr(indirect_block_addr[ind]);
                    if(indirect_addr == NULL)  {
                       ERROR("bad indirect address in inode");
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
   }
   
   if(references <= 0) {
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
           if(i->addrs[ind] == 0) 
               break; 
           addr = (struct dirent *)getAddr(i->addrs[ind]);
           if(addr == NULL)  {
             ERROR("bad direct address in inode");
           }
          for(j = addr; j < &addr[NUM_ENTRIES]; j ++) {
           if(j->inum  == inum){
             dir_reference++;
           }
          }
       }

       if(i->addrs[NDIRECT] != 0) {       

       indirect_block_addr  = (uint *) getAddr(i->addrs[NDIRECT]);
        if(indirect_block_addr == NULL)  {
          ERROR("bad indirect address in inode");
       }        
       for(ind = 0; ind < NINDIRECT; ind++) {
          if(indirect_block_addr[ind] == 0) {
            break;
          }
          indirect_addr = (struct dirent *)getAddr(indirect_block_addr[ind]);
           if(indirect_addr == NULL)  {
              ERROR("bad indirect address in inode");
           }
             for(j = indirect_addr; j < &indirect_addr[NUM_ENTRIES] ; j++) {
                if(j->inum  == inum){
                   dir_reference++;
                }
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

  inode_inuse(ip, inum);

}


