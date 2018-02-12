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
  printf(1, "Hello "); 
  int* x = malloc(sizeof(int));
  *x = 2; 
  f();
}
