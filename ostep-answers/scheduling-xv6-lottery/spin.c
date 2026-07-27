#include "types.h"
#include "user.h"

int 
main(int argc, char* argv[])
{
  int i;
  int x = 0;

  if (argc > 3 || argc < 2) {
    exit();
  }

  if (argc == 3) {
    settickets(atoi(argv[2]));
  }

  for(i = 1; i < atoi(argv[1]); i++){
    x += 1;
  }
  exit();

  return x;
}
