#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define assert(x) if (x) {} else { \
  printf(1, "%s: %d ", __FILE__, __LINE__); \
  printf(1, "assert failed (%s)\n", # x); \
  printf(1, "TEST FAILED\n"); \
  exit(); \
}

int
main(int argc, char *argv[])
{
  uint sz = (uint) sbrk(0);
  uint stackpage = (160 - 1) * 4096;
  uint guardpage = stackpage - 5 * 4096;

  // ensure they actually placed stack high...
  *(char*)stackpage = 'a';

  // full use of heap possible
  assert((int) sbrk(guardpage - sz) != -1);

  // but not into guardpage
  assert((int) sbrk(1) == -1);
printf(1, "fuck you 1\n"); 
  assert((int) sbrk(1*4096) == -1);
printf(1, "fuck you 2\n"); 
  assert((int) sbrk(5*4096-1) == -1);
printf(1, "fuck you 3\n"); 
  assert((int) sbrk(5*4096) == -1);
printf(1, "fuck you 4\n"); 
  assert((int) sbrk(5*4096+1) == -1);
printf(1, "fuck you 5\n"); 
  assert((int) sbrk(6*4096) == -1);
printf(1, "fuck you 6\n"); 
  assert((int) sbrk(-1*(guardpage - sz)) != -1);
printf(1, "fuck you 7\n"); 
  assert((int) sbrk(guardpage - sz + 1) == -1);
printf(1, "fuck you 8\n"); 
  assert((int) sbrk(guardpage - sz + 1*4096) == -1);
printf(1, "fuck you 9\n"); 
  assert((int) sbrk(guardpage - sz + 5*4096-1) == -1);
printf(1, "fuck you 10\n"); 
  assert((int) sbrk(guardpage - sz + 5*4096) == -1);
printf(1, "fuck you 11\n"); 
  assert((int) sbrk(guardpage - sz + 5*4096+1) == -1);
printf(1, "fuck you 12\n"); 
  assert((int) sbrk(guardpage - sz + 6*4096) == -1);
printf(1, "fuck you 13\n"); 
  assert((int) sbrk(guardpage - sz) != -1);
  printf(1, "TEST PASSED\n");
  exit();
}
   
