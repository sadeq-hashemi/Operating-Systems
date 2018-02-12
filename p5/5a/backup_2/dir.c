#include "xv6_fsck.h"

//looks through direntries and finds arg inum
int 
check_parent(struct dinode *iparent, uint child_inum) {
  uint *addrs;
  uint *indaddrs;
  struct dirent* entry_start, entry;
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
    for(entry = entry_start; entry < entry_start[NUM_ENTRIES]; entry++){
      if(entry->inum == child_inum)
        return 0;
    }
  }

  if(addrs[NDIRECT + 1] == 0)
    return -1;
  indaddrs = (uint*) getAddr(addrs[NDIRECT + 1]);
  if (indaddrs == NULL )
    ERROR("bad direct address"); 
  for(int i = 0; i < NINDIRECT; i++) {
    if(indaddrs[i] == 0)
      return -1;

    entry_start = (struct dirent*) getAddr(indaddrs[i]);
    if (entry_start == NULL )
      ERROR("bad indirect address");
    for(entry = entry_start; entry < entry_start[NUM_ENTRIES]; entry++){
      if(entry->inum == child_inum)
        return 0;  
  }
  return -1;
}

//Checking inum 1
int
check_root(struct dinode* ip, uint inum) {
//inum is 1
uint* addrs;
struct dirent *entry;

  if ( inum == 1 ) {
    if(ip->type != T_DIR)
      return 0;

    addrs = ip->addrs;
    dirent *entry = (struct dirent*) getAddr(addrs[i]); 
    if (entry == NULL)
      ERROR("bad direct address in inode");

    if(entry->inum == 1 && strcmp(entry->name, ".") == 0)
      if(entry[1].inum == 1 && strcmp(entry[1].name, "..") == 0)
        return 1;
  }

  return 0;
}

int
check_reg_dir(struct dinode* ip, uint inum) {
 int ind, root_found, found;
 uint* indaddr;
 uint* addrs, *addrs_temp;
 uint inum_entry;
 void* data;
 struct dirent *self, *parent, *dirent_start, *entry, *temp_dirent;
 struct dinode* temp_inode;
 
  root_found = check_root(struct dinode* ip, uint inum);

  addrs = ip->addrs;
  for(int i = 0; i < NDIRECT; i++) {
    //checks for duplicates
    for(int x = 0; i < NDIRECT; i++)
      if (i != x)
        if(addrs[i] == addrs[x])
          ERROR("direct address used more than once");

    //checks validity of bn
    dirent_start = (struct dirent*) getAddr(addrs[i]);
    if(dirent_start == NULL)
      ERROR("bad direct address in inode");


    for(entry= dirent_start; entry < dirent_start[NUM_ENTRIES]; entry++){

      //checks self 
      if((entry - dirent_start) == 0) { //case to check for self
        if((entry->inum != inum) || (strcmp((entry->name), ".")) != 0)
          ERROR("directory not properly formatted");
      }

      //checks parent
      if((entry - dirent_start) == 1) { //case to check for parent
        if((entry->inum == 0) || (strcmp((entry->name), "..")) != 0)
          ERROR("directory not properly formatted");
          temp_inode = &ntable[entry->inum];
          if((temp_inode != T_DIR) || (check_parent(temp_inode, inum) < 0))
            ERROR("parent directory mismatch");
      }

      //anything else?
      
    }

  }
  indaddrs = (unit*) getAddr(addrs[NDIRECT + 1]);
  if (indaddrs == NULL)
    ERROR("bad indirect address in inode");
  for(int i = 0; i < NINDIRECT; i++) {

    for(int x = 0; i < NINDIRECT; i++)
      if (i != x)
        if(addrs[i] == addrs[x])
          ERROR("indirect address used more than once");
    
  }


  //TODO do something with root_found




  self = (struct dirent*) getAddr(addrs[0]);
  parent = (struct dirent*) getAddr(addrs[1]);
  //parent case
  //check if parent points back

  //rest of entries
  ind = 2; //start with parent
  while(addrs[ind] != 0 && ind < NDIRECT) {
    //checks for repetitions
    dirent = (struct dirent*) getAddr(addrs[ind]);
    for(uint* i = ip->addrs; i < addrs[NDIRECT]; i++){
      if((i - ip->addrs) == ind)
        continue;
      temp_dirent = (struct dirent*) getAddr(i);
      if(temp_dirent == NULL)
    }
  if (dirent == NULL) 
  inum_entry = dirent->inum;  
  ind++;
  }
 //if inum 1 is not root dir root = 0;
  if ( inum == 1 ) {
      
  }
  //check if parent is valid and points back

  //each direct addr is used ONLY once
    //each inode is also marked valid

  //each indirect is only used once
    //each inode is also marked valid

  //this directory only appears once
}
void
check_dir(struct dinode *ip, uint inum) {
  if(ip == NULL) {
    ERROR("invalid ip in check_dir()");
  }
  if(ip->type != T_DIR) {
    ERROR("ip is not of type T_DIR");
  }
  if(addrs[0] == 0 || addrs[1] == 0)
    ERROR("directory not properly formatted");
  if(inum == 1) {
    check_root();
  } else if (inum > 1) {
    check_reg_dir();
  }
}
