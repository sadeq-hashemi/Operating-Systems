#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
// TODO(student): Include necessary headers

#define SEG_SIZE 64

typedef struct {
  int valid; 
  int pid; 
  char birth[25];
  int start_sec;
  double start_usec;
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
    // TODO(student): Clear the shared memory segment
// FREE map;
    close(fd_shm); 
    int ret = shm_unlink(SHM_NAME);
//    free(map); 
    map = NULL;
    mutex = NULL;
// ASK 
//   int ret = rmdir(path_name);
      if ( ret < 0 ) {
        exit(1);
      }

    exit(0);
}

int main(int argc, char **argv) {
    int ret;
    // TODO(student): Signal handling
    // Use sigaction() here and call exit_handler
    struct sigaction act; 
    memset(&act, '\0', sizeof(act)); 
    act.sa_handler = &exit_handler; 
    sigaction( SIGINT ,&act, NULL); 
    
    // TODO(student): Create a new shared memory segment
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);
    if ( fd_shm < 0 ) {
      perror("shm_open\n");
      exit(1);
    }
    ret = ftruncate(fd_shm, length);
    if ( ret < 0 ) {
      perror("tfruncate\n");
      exit(1);
    }
    map = mmap(NULL, (size_t) length, PROT_READ | PROT_WRITE,
      MAP_SHARED, fd_shm, 0);
    if ( map == NULL) {
      perror("mmap\n");
      exit(1);
    }
    // TODO(student): Point the mutex to the segment of the shared memory page
    // that you decide to store the semaphore
    mutex = map;
    ret = sem_init(mutex, 1, 1);  // Initialize semaphore
      if ( ret < 0 ) {
        perror("sem_init\n");
        exit(1); 
      }
    // TODO(student): Some initialization of the rest of the segments in the
    // shared memory page here if necessary
    for ( int x = 1; x < 64; x ++ ) {
      stats_t *new; 
      new = map + (x*SEG_SIZE);
      new->valid = 1;
      memset(new->birth, '\0', sizeof(new->birth));
      new->start_sec = 0; 
      new->start_usec = 0.0; 
      new->elapsed_sec = 0; 
      new->elapsed_msec = 0.0; 
    }
    while (1) {
        // TODO(student): Display the statistics of active clients, i.e. valid
        // segments after some formatting
        sleep(1);
        for (int x= 1; x < 64; x++ ) {
          stats_t *p; 
          p = map + (x*SEG_SIZE);
          if(!p->valid) { 
            printf("pid : %d, birth : %s, elapsed : %d s %.4f ms\n", p->pid,
              p->birth, p->elapsed_sec, p->elapsed_msec);
            }
          p = NULL; 
        }
    }

    return 0;
}

