#include "types.h"
#include "defs.h"

// Implementation defined in "The C Standard Library" by P.J. Plauger

static unsigned long _Randseed = 1;

int 
rand(void)
{
  _Randseed = _Randseed * 1103515245 + 12345;
  return ((unsigned int)(_Randseed >> 16) & RAND_MAX);
}
