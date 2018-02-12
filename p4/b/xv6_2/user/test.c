#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int global = 1;

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

   int arg = 42;
   printf(1, "printing ppid %d\n", ppid);
   int clone_pid = clone(worker, &arg);
   printf(1, "printing clone_pid %d\n", clone_pid);
   assert(clone_pid > 0);

   sleep(250);
   assert(wait() == -1);
   printf(1, "after wait");

   int join_pid = join();
   printf(1, "printing join_pid = %d\n", join_pid);
   assert(join_pid == clone_pid);
   assert(global == 2);

   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
   printf(1, "clone starting ...\n");
   int arg = *(int*)arg_ptr;
   assert(arg == 42);
   assert(global == 1);
   global++;

   printf(1, "clone ending ...\n");
}


