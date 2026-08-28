#include "types.h"
#include "sysfunc.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "defs.h"


int
sys_mprotect(void)
{
  void* addr;
  int len;
  // do we have a valid pointer 
  if (argptr(0, (void*)&addr, PGSIZE) < 0) {
     return -1;  
  }
  // do we have a valid int
  if (argint(1, &len) < 0) {
    return -1;
  }  

  if (mprotect(addr, len) < 0){
    return -1;
  }

  return 0;
}

int 
sys_munprotect(void)
{
  void* addr;
  int len;

  // do we have a valid pointer 
  if (argptr(0, (void*)&addr, PGSIZE) < 0) {
     return -1;  
  }
  // do we have a valid int
  if (argint(1, &len) < 0) {
    return -1;
  }  

  if (mprotect(addr, len) < 0){
    return -1;
  }

  return 0;
}

