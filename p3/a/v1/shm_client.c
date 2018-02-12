#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
// TODO(student): Include necessary headers

#define SEG_SIZE 64

typedef struct {
  int valid;
  int pid;
  char birth[25];
  int start_time;
  int elapsed_sec;
  double elapsed_msec;
} stats_t;

const char * SHM_NAME = "sadeq_patri";
const char * path_name = "/dev/shm/sadeq_patri";
const off_t length = 4096;
sem_t * mutex;
void *map;
int fd_shm;


void exit_handler(int sig) {
    // new routine defined here specified by sigaction() in main
    // TODO(student): critical section begins
        sem_wait(mutex);
    // client reset its segment, or mark its segment as valid
    // so that the segment can be used by another client later.
        sem_post(mutex);
    // critical section ends
    exit(0);
}

char *get_time (  ) {
 
    time_t rawtime;
    struct tm * timeinfo;
    struct tm   timeinfoBuffer;
    char *result;
    
    time(&rawtime);
    /* call localtime */
    timeinfo = localtime_r(&rawtime , &timeinfoBuffer);
    /* allocate memory for the result of asctime call*/
    result = malloc(26 * sizeof(char));
    /* call reentrant asctime function */
    result = asctime_r(timeinfo, result);

    return result; 
}

int main(int argc, char *argv[]) {
    // TODO(student): Signal handling
    // Use sigaction() here and call exit_handler
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = &exit_handler;
    sigaction( SIGINT ,&act, NULL);
    // TODO(student): Open the preexisting shared memory segment created by
    // shm_server
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_EXCL, 0660);
      if ( fd_shm < 0 ) { 
        perror("shm_open\n");
        exit(1); 
      }    
    // TODO(student): point the mutex to the particular segment of the shared
    // memory page
    map = mmap(NULL, (size_t) length, PROT_READ | PROT_WRITE, 
      MAP_SHARED, fd_shm, 0); 
    if ( map == NULL) {
      perror("mmap\n");
      exit(1);
    }
    mutex = map; 

    // TODO(student): critical section begins
        sem_wait(mutex);
        // client searching through the segments of the page to find a valid
        // (or available) segment and then mark this segment as invalid
        for ( int ind = 1; ind < SEG_SIZE; ind ++ ) {
          stats_t *p;
          p = map + (ind*SEG_SIZE);
          if ( p->valid ) { 
             p->pid = getpid();
             (void) strcpy((char*)&(p->birth), get_time());  
// TEST printf("%d, %s\n",p->pid,  p->birth); 
             
             break;
          }
     }
        sem_post(mutex);
    // critical section ends


    while (1) {
        // TODO(student): fill in fields in stats_t
        sem_wait(mutex);
        sleep(1);
        (void) get_time(); 
        // display the PIDs of all the active clients
    }

    return 0;
}


