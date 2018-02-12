#include <unistd.h>
#include <string.h> 
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
 
int
main(int argc, char* argv[]) {

int ret; 


  ret = fork(); 

  if ( ret == 0 ) { 
    char *server[2];
    server[0] = strdup("/afs/cs.wisc.edu/u/s/a/sadeq/private/cs537/p3/a/v3/shm_server");
    server[1] = NULL;
    execv(server[0], server);
    perror("exec"); 
    exit(1);  
  } else if ( ret > 0 ) { 
      ret = fork(); 
      if ( ret == 0 ) { 
        char *client[2];
        client[0] = strdup("/afs/cs.wisc.edu/u/s/a/sadeq/private/cs537/p3/a/v3/shm_client");
        client[1] = NULL;
        execv(client[0], client); 
        perror("exec"); 
        exit(1);  
     } else if ( ret > 0 ) {
        exit(0);  
      } else {
        perror("fork"); 
        exit(1); 
      }
 } else {
    perror("fork"); 
    exit(1); 
  }


return 0; 
}
