#include "xv6_fsck.h"
void
check_extralink() {
  
  int  extra_reference = 0;
  struct dinode *i;
  uint ind, inum;
  uint *indirect_block_addr;
  struct dirent *j, *addr, *indirect_addr;
 	 
  for(i = ntable; i < &ntable[sb->ninodes]; i ++) {
    extra_reference = 0;
    inum = i - ntable;
    if(i->type == T_DIR) {

      for(ind = 0; ind <NDIRECT; ind++) {
        if(i->addrs[ind] == 0) 
          break;
        addr = (struct dirent *)getAddr(i->addrs[ind]);
        if(addr == NULL)  {
          ERROR("bad direct address in inode");
          }
        for(j = addr; j < &addr[NUM_ENTRIES]; j++) {

          if(j->inum  == inum){
            extra_reference++;
           }
          }
       	}

        if(i->addrs[NDIRECT] != 0) {                       
      	  indirect_block_addr  = (uint *) getAddr(i->addrs[NDIRECT]);
          if(indirect_block_addr == NULL)  {
            printf("bn: %u", i->addrs[NDIRECT + 1]);
            ERROR("bad indirect address in inode");
       	    }
       	  for(ind = 0; ind < NINDIRECT; ind++) {
            if(indirect_block_addr == 0) {
              break;
              }
            indirect_addr = (struct dirent *)getAddr(indirect_block_addr[ind]);
            if(indirect_addr == NULL)  {
              ERROR("bad indirect address in inode");
              }
            for(j = indirect_addr; j < &indirect_addr[NUM_ENTRIES] ; j++) {
              if(j->inum  == inum){
                extra_reference++;
                 }
                }
       	      }
     	    }
         if(inum == 1) {
         if(extra_reference > 2) {
           ERROR("directory appears more than once in file system");
           }
 
         } else {
         if(extra_reference > 1) {
           ERROR("directory appears more than once in file system");
           }
         }
      }
   }
}
//looks through direntries and finds arg inum
int 
check_parent(struct dinode *iparent, uint child_inum) {
  uint *addrs;
  uint *indaddrs;
  struct dirent* entry_start, *entry;

  addrs = (uint*)iparent->addrs;

//TRACK SIZE
//finds parent, and checks if child_inum exists in it
  for(int i= 0; i < NDIRECT; i++) {
    if(addrs[i] == 0)
      return -1;
    entry_start = (struct dirent*) getAddr(addrs[i]);
    if (entry_start == NULL ) {
      ERROR("bad direct address");
    }
    for(entry = entry_start; entry < &entry_start[NUM_ENTRIES]; entry++){
      if(entry->inum == child_inum)
        return 0;
    }
  }

  if(addrs[NDIRECT] == 0)
    return -1;
  indaddrs = (uint*) getAddr(addrs[NDIRECT]);
  if (indaddrs == NULL )
    ERROR("bad direct address"); 
  for(int i = 0; i < NINDIRECT; i++) {
    if(indaddrs[i] == 0)
      return -1;

    entry_start = (struct dirent*) getAddr(indaddrs[i]);
    if (entry_start == NULL )
      ERROR("bad indirect address");
    for(entry = entry_start; entry < &entry_start[NUM_ENTRIES]; entry++){
      if(entry->inum == child_inum)
        return 0;  
  }
  }
  return -1;
}

//Checking inum 1
void
check_root() {
//inum is 1
uint* addrs;
struct dinode *ip;
struct dirent *entry;

  ip = &ntable[1];
 
    if(ip->type != T_DIR)
      ERROR("root directory does not exist");
    addrs = ip->addrs;
    entry = (struct dirent*) getAddr(addrs[0]); 
    if (entry == NULL)
      ERROR("bad direct address in inode");

    if(entry->inum == 1 && strcmp(entry->name, ".") == 0)
      if(entry[1].inum == 1 && strcmp(entry[1].name, "..") == 0)
        return;
    ERROR("root directory does not exist");
}

void
check_reg_dir(struct dinode* ip, uint inum) {
 uint temp;
 uint* indaddrs;
 uint* addrs;
 struct dirent  *dirent_start, *entry;
 struct dinode* temp_inode;
 
  addrs = ip->addrs;
//DIRECT
  for(int i = 0; i < NDIRECT; i++) {
    //checks for duplicates
    if(addrs[i] == 0)
      break;

    for(int x = 0; x < NDIRECT; x++) {
      if(addrs[x] == 0)
        break;
      if (i != x)
        if(addrs[i] == addrs[x])
          ERROR("direct address used more than once");
    }
    //checks validity of bn
    dirent_start = (struct dirent*) getAddr(addrs[i]);
    if(dirent_start == NULL)
      ERROR("bad direct address in inode");

    if (i == 0) {
      for(entry= dirent_start; entry < &dirent_start[NUM_ENTRIES]; entry++){

        //checks self 
        if((entry - dirent_start) == 0) { //case to check for self
          if((entry->inum != inum) || (strcmp((entry->name), ".")) != 0)
            ERROR("directory not properly formatted");
        }

        //checks parent
        if((entry - dirent_start) == 1) { //case to check for parent
         if(inum == 1) {  //root 
            if((entry->inum == 0) || entry->inum != inum || (strcmp((entry->name), "..")) != 0)
            ERROR("directory not properly formatted");
            temp_inode = &ntable[entry->inum];
            if((temp_inode->type != T_DIR) || (check_parent(temp_inode, inum) < 0))
              ERROR("parent directory mismatch");
          } else {
            if((entry->inum == 0) || entry->inum == inum || (strcmp((entry->name), "..")) != 0)
              ERROR("directory not properly formatted");
            temp_inode = &ntable[entry->inum];
            if((temp_inode->type != T_DIR) || (check_parent(temp_inode, inum) < 0))
              ERROR("parent directory mismatch");
          }
          }
        }
       }
      for(entry = dirent_start; entry < &dirent_start[NUM_ENTRIES]; entry++) {
        //checks valid in bmap
        temp = entry->inum; //inode number
        if(temp == 0)
          break;
        temp_inode = &ntable[temp];
        if(temp_inode->type == 0) {
          ERROR("inode referred to in directory but marked free");
          }
        }
    }
  

//INDIRECT
  if(addrs[NDIRECT] == 0)
    return;
  indaddrs = (uint*) getAddr(addrs[NDIRECT]);
  if (indaddrs == NULL)
    ERROR("bad indirect address in inode");

  for(int i = 0; i < NINDIRECT; i++) {

      if(indaddrs[i] == 0)
        return;

    for(int x = 0; i < NINDIRECT; i++) {
      if(indaddrs[x] == 0)
        break;
      if (i != x)
        if(addrs[i] == addrs[x])
          ERROR("indirect address used more than once");
    }

    //checks validity of bn
    dirent_start = (struct dirent*) getAddr(indaddrs[i]);
    if(dirent_start == NULL)
      ERROR("bad indirect address in inode");

       for(entry = dirent_start; entry < &dirent_start[NUM_ENTRIES]; entry++) {
        //checks valid in bmap
        temp = entry->inum; //inode number
        if(temp == 0)
          break;
        temp_inode = &ntable[temp];
        if(temp_inode->type == 0) {
          ERROR("inode referred to in directory but marked free");
          }
        }
   
  }
}

void
check_dir(struct dinode *ip, uint inum) {
  if(ip == NULL) {
    ERROR("invalid ip in check_dir()");
  }
  if(ip->type != T_DIR) {
    ERROR("ip is not of type T_DIR");
  }
  if(ip->addrs[0] == 0)
    ERROR("directory not properly formatted");
  check_reg_dir(ip, inum);  
}
