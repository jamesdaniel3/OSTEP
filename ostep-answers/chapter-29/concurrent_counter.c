/*
build a simple concurrent counter and measure how long it
takes to increment the counter many times as the number of threads
increases. How many CPUs are available on the system you are
using? Does this number impact your measurements at all?

On a system with 4 CPU's, I see these results:

Time to increment counter 10000000 times with 1 threads: 0.6546028280
Time to increment counter 10000000 times with 2 threads: 3.2159286290
Time to increment counter 10000000 times with 3 threads: 1.8604205610
Time to increment counter 10000000 times with 4 threads: 1.9794236170
Time to increment counter 10000000 times with 5 threads: 1.8677156230

I can't quite explain the trend that the results show. Best guess if that 
there is some overhead caused by spliting the task between cpus/threads, but 
all of the overhead is seen in the jump to two threads, because if you are 
switching between two cpus and three cpus, there is no material difference as 
you are still switching between cpus every time you change threads. Once you have 
absorbed the overhead maybe it helps to have more threads up to a point?

Either way, the number of CPUs would be relevant when testing because if you 
have more threads than CPUs than the threads cannot run concurrently. 
*/

#include <time.h>
#include <pthread.h> // compilation will require -pthread
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define BILLION 1000000000.0
#define TOTAL 10000000

pthread_mutex_t lock;
long counter;


void* grow_counter(void* iterations_ptr){
  size_t iterations = *(size_t *)iterations_ptr;
  for(size_t i = 0; i < iterations; i++) {
     pthread_mutex_lock(&lock);
     counter++;
     pthread_mutex_unlock(&lock);
  }

  return NULL;
}


int main(){
  int rc = pthread_mutex_init(&lock, NULL);
  assert(rc == 0);

  struct timespec start, end;

  for(size_t num_threads = 1; num_threads <= 5; num_threads++){
    size_t count_per_thread = TOTAL / num_threads;
    size_t remainder = TOTAL % num_threads;    

    pthread_t threads[5];
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(size_t j = 0; j < num_threads; j++){  
      int rc = pthread_create(threads + j, NULL, grow_counter, &count_per_thread);
      assert(rc == 0);
    }

    for(size_t j = 0; j < num_threads; j++){
      pthread_join(threads[j], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    
    assert(counter == TOTAL || counter + remainder == TOTAL);
    printf("Time to increment counter %d times with %ld threads: %.10f \n", TOTAL, num_threads, time_spent);    
    counter = 0;
  }

  return 0;
}
