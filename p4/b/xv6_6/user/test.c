#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int global = 0;
sem_t mutex;
int num_threads = 8;
int loops = 1000;


#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();

   sem_init(&mutex, 1);

   int i;
   for (i = 0; i < num_threads; i++) {
      int thread_pid = clone(worker, 0);
      assert(thread_pid > 0);
   }

   for (i = 0; i < num_threads; i++) {
      int join_pid = join();
      assert(join_pid > 0);
   }
   printf(1, "global= %d expected: %d\n", global, num_threads*loops);
   assert(global == num_threads * loops);
   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
   int i, tmp;
  // printf(1, "num threads pid = %d \n" , getpid());
   for (i = 0; i < loops; i++) {
      sem_wait(&mutex);
      printf(1, "num threads pid = %d, global: %d, tmp, %d \n" , getpid(), global, tmp+1);
      tmp = global;
      open("README", O_WRONLY | O_CREATE);
      global = tmp + 1;
      //printf(1, "global during worker  = %d \n");
      sem_post(&mutex);
   }
   exit();
}

