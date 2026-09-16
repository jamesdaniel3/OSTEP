/*  TEST RESULTS: 

Basic Implementation: 1 Global Lock Used for all Relevant operations

Time to search for 20000 values with 1 threads: 1.7071923630
Time to search for 20000 values with 2 threads: 2.4495175210
Time to search for 20000 values with 3 threads: 2.4950721470
Time to search for 20000 values with 4 threads: 2.5344956870
Time to search for 20000 values with 5 threads: 2.5632890820
Time to search for 20000 values with 6 threads: 2.5951465180
Time to search for 20000 values with 7 threads: 2.6412031480
Time to search for 20000 values with 8 threads: 2.6784806550

Advanced Implementation: 1 Lock Per Node

*/


#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../rng64.h"
#include "btree.h"

#define NUM_VALUES 2000000
#define NUM_OPERATIONS 200000
#define BILLION 1000000000.0
#define MAX_THREAD_COUNT 8

// feels like this should be replaced with one struct list tbh
int searches[NUM_OPERATIONS];
tree_node* head;

typedef struct runner_args runner_args;
struct runner_args{
    size_t starting_index;
    size_t num_operations;
};

void* run_operations(void* args){
    runner_args args_unpacked = *(runner_args *)args;

    size_t num_found = 0;

    for(size_t i = 0; i < args_unpacked.num_operations;i++){
        int argument = searches[args_unpacked.starting_index + i];
        num_found += tree_search(head, argument);
    }

    printf("Num Found: %zu\n", num_found);

    return NULL;
}

int main(){
    rng64_randomize();
    rng64_intrange_spec nums;
    rng64_set_intrange(&nums, INT_MIN, INT_MAX);
    
    head = init_node();

    for(size_t i = 0; i < NUM_VALUES; i++){
        int random_number = (int) rng64_intrange(&nums);
        tree_insert(head, random_number);
    }
    
    for(size_t i = 0; i < NUM_OPERATIONS; i++){
        int random_arg = (int) rng64_intrange(&nums);
        searches[i] = random_arg;
    }

    struct timespec start, end;

    for(size_t num_threads = 1; num_threads <= MAX_THREAD_COUNT; num_threads++){
        size_t count_per_thread = NUM_OPERATIONS / num_threads; // losing remainder

        pthread_t threads[MAX_THREAD_COUNT];
        runner_args args[MAX_THREAD_COUNT];
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(size_t j = 0; j < num_threads; j++){
            args[j].starting_index = count_per_thread * j;
            args[j].num_operations = count_per_thread;

            int rc = pthread_create(threads + j, NULL, run_operations, args + j);
            assert(rc == 0);
        }

        for(size_t j = 0; j < num_threads; j++){
            pthread_join(threads[j], NULL);
        }
    
        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

        printf("Time to search for %d values with %ld threads: %.10f \n", NUM_OPERATIONS, num_threads, time_spent);    

    }
}
