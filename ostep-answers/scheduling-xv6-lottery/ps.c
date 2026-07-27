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

int
main(int argc, char *argv[])
{
  // eventually this program should take args that allow users to specify a single 
   // process that they want info on but for now I am just making it output everything
   // also not concerned with hiding processes from users etc atm

  ps();
  exit();
}
