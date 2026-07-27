#include "types.h"
#include "user.h"
#include "pstat.h"
  
void
ps()
{
    struct pstat result; // this needs to be initialized
    getpinfo(&result);

    for(int i = 0; i < NPROC; i++){
      if (!result.inuse[i]) {
          continue;
      }

      printf(1, "PID %d: %d tickets, %d ticks accumulated\n", result.pid[i], result.tickets[i], result.ticks[i]);
    }
 
 }

int iterate(){
  volatile int x = 0;
  for(;;){
    x++;
  }
}

int 
main(int argc, char* argv[])
{

  int pid1 = fork();

  if (pid1 == 0) {
    settickets(1);
    iterate(); 
  }
  
  else {
    int pid2 = fork();

    if (pid2 == 0){
      settickets(2);
      iterate();
    }

    else {
	int pid3 = fork();

	if (pid3 == 0) {
	  settickets(3);
 	  iterate();
	}

	else {
	  sleep(200);
	  for(int i = 0; i < 20; i++){
	    ps();
	    printf(1, "________________");
	    sleep(500);
	  }  
	}
    }
  }
  exit();
}
