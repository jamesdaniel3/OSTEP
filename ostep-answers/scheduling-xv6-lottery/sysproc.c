#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"

int
sys_fork(void)
{
  return fork();
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
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
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
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
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
sys_settickets(void)
{
  int desired_num_tickets;
 
  if (argint(0, &desired_num_tickets) < 0) {
	return -1;
  }

  if (desired_num_tickets < 1) {
	return -1;
  }

  myproc()->num_tickets = desired_num_tickets;
  return 0;
}

int 
sys_getpinfo(void)
{
  struct pstat * result;

  if (argptr(0, (void*)&result, sizeof(struct pstat)) < 0) {
    return -1;
  }  

  // fill in struct. Similar to total tickets; I think it would be better to just 
    // store all of this info globally so it doesn't have to be caculated when 
    // the function is called but I want to come back to that as it has more pitfalls

  calculate_pstat(result);  
   
  return 0;
}
