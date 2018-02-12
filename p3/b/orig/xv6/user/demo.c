#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
void f() { 
  printf(1, "World \n");
  exit();
}
int
main(int argc, char *argv[])
{
void* prev; 
void* curr; 
  printf(1, "Hello\n"); 
  prev = sbrk(0); 
  printf(1, "Prev sbrk output: %p\n", prev); 
//  int* x; 
 // x = malloc(sizeof(int));
  curr = sbrk(0);
  printf(1, "Curr sbrk output: %p\n", curr); 
//  x = malloc(10*4096); 
  curr = sbrk(10);
  curr = sbrk(0); 
  printf(1, "Curr sbrk output: %p\n", curr); 
//  *x = 2; 
  f();
}
