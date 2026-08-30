/*
build a version of the approximate counter. Once again, measure its performance 
as the number of threads varies, as well as the
threshold. Do the numbers match what you see in the chapter?

Time to increment counter 10000000 times with 1 threads: 0.7969034560
Time to increment counter 10000000 times with 2 threads: 1.5698332670
Time to increment counter 10000000 times with 3 threads: 1.0738999130
Time to increment counter 10000000 times with 4 threads: 0.8806807100
Time to increment counter 10000000 times with 5 threads: 1.4380607190

These don't really match what happened in the chater. There is still that weird
spike at 2 which I am tempted to blame on the VM but I really don't know yet.
There is a noticably decreased perfomance impact with the new implementation.
*/

#include <time.h>
#include <pthread.h> // compilation will require -pthread
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define BILLION 1000000000.0
#define TOTAL 10000000
#define NUM_CPUS 4

const int THRESHOLD = 1024;
pthread_mutex_t global_lock;
long global_counter;
pthread_mutex_t local_locks[NUM_CPUS];
long local_counts[NUM_CPUS];

typedef struct grow_counter_args grow_counter_args;
struct grow_counter_args {
  size_t iterations;
  size_t thread_num;
};

void* grow_counter(void* args){
  grow_counter_args args_unpacked = *(grow_counter_args *)args;
  for(size_t i = 0; i < args_unpacked.iterations; i++) {
     size_t index = args_unpacked.thread_num % NUM_CPUS; // not needed but safer
     pthread_mutex_lock(&local_locks[index]); 
     local_counts[index]++;

     if (local_counts[index] >= THRESHOLD) {
         pthread_mutex_lock(&global_lock);
         global_counter += local_counts[index];
         local_counts[index] = 0;
         pthread_mutex_unlock(&global_lock);
     }

     pthread_mutex_unlock(&local_locks[index]);
  }

  return NULL;
}


int main(){
  int rc = pthread_mutex_init(&global_lock, NULL);
  assert(rc == 0);

  for(size_t i = 0; i < NUM_CPUS; i++){
    rc = pthread_mutex_init(&local_locks[i], NULL);
    assert(rc == 0);
  } 

  struct timespec start, end;

  for(size_t num_threads = 1; num_threads <= 5; num_threads++){
    size_t count_per_thread = TOTAL / num_threads;
    size_t remainder = TOTAL % num_threads;    

    pthread_t threads[5];
    grow_counter_args args[5];
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(size_t j = 0; j < num_threads; j++){
      args[j].iterations = count_per_thread;
      args[j].thread_num = j;

      int rc = pthread_create(threads + j, NULL, grow_counter, args + j);
      assert(rc == 0);
    }

    for(size_t j = 0; j < num_threads; j++){
      pthread_join(threads[j], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("Time to increment counter %d times with %ld threads: %.10f \n", TOTAL, num_threads, time_spent);    

    size_t result = global_counter;
    for (size_t i = 0; i < NUM_CPUS; i++){
        result += local_counts[i];
    }
    assert(result == TOTAL - remainder);

    global_counter = 0;
    for (size_t i=0; i < NUM_CPUS; i++){
        local_counts[i] = 0;
    }
  }

  return 0;
}
