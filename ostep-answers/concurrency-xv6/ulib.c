#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

void 
lock_init(lock_t* lock)
{
    // it should probs be enforced that init can only be called once per lock
    // didn't handle null pointers b/c nothing else is; program will crash
    lock->ticket = 0;
    lock->turn = 0;
}

int 
_fetch_and_add(int* var, int val)
{
    asm volatile(
        "lock; xaddl %0, %1"
        : "+r" (val), "+m" (*var) // input and output 
        : // No input-only
        : "memory"
    );

    return val; // xaddl swaps the old value of var into val
}

void 
lock_acquire(lock_t* lock)
{
    int my_turn = _fetch_and_add(&lock->ticket, 1);
    while (lock->turn != my_turn){
        // spin
        continue;
    } 
}

void 
lock_release(lock_t* lock)
{
    lock->turn += 1;
}

int 
thread_create(void(*start_routine)(void*, void*), void* arg1, void* arg2)
{
   void* child_stack = malloc(4096); // I don't think PGSIZE is here
   
   if (!child_stack){
        return -1;
   }

   return clone(start_routine, arg1, arg2, child_stack);
}

int 
thread_join()
{
    void* user_stack;
    int pid = join(&user_stack);

    if (!user_stack){
        pid = -1;
    }

    if (pid != -1 && user_stack){
        free(user_stack);
    }

    return pid;
}

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
