#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "spinlock.h"
int
sys_fork(void)
{
  return fork();
}

int
sys_clone(void)
{
  void(*fcn)(void*);
  void *arg;  
  if(argptr(0, (void *)&fcn, sizeof(fcn)) < 0){
     return -1;
  }
  if(argptr_clone(1, (void *)&arg, sizeof (void*)) < 0) {
    return -1; 
}
return clone(fcn, arg);
}

int 
sys_join(void) {

return join(); 
}


int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  
  int addr;
  int n;
  struct proc* mainth;  
  if(argint(0, &n) < 0)
    return -1;

  mainth = proc; 
  while( mainth->mainthread != 1) 
    mainth = proc->parent; 

  if(mainth->sz != proc->sz) {
    if(mainth->sz > proc->sz) {
      proc->sz = mainth->sz;
    } else {
      mainth->sz = proc->sz;
    }
  }

   addr = proc->sz;
 if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_sleep2(void)
{
  void* cnd;
  void *lk;  
  if(argptr(0, (void *)&cnd, sizeof(cnd)) < 0){
     return -1;
  }
  if(argptr(1, (void *)&lk, sizeof (void*)) < 0) {
    return -1; 
}
return sleep2(cnd, lk);
}

int
sys_wakeup2(void)
{
   void* cnd;
  if(argptr(0, (void *)&cnd, sizeof (void*)) < 0) {
    return -1; 
}
return wakeup2(cnd);
}



