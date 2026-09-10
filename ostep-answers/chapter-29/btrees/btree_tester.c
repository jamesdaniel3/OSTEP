#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "../rng64.h"
#include "btree.h"

#define NUM_VALUES 200000
#define NUM_OPERATIONS 5000
#define BILLION 1000000000.0
#define MAX_THREAD_COUNT 4

// feels like this should be replaced with one struct list tbh
int operation_list[NUM_OPERATIONS];
int operation_arg_list[NUM_OPERATIONS];
tree_node* head;

enum {
    TREE_SEARCH = 0,
    TREE_INSERT = 1,
    TREE_DELETE = 2
} FUNCTION_LIST_INDEXES;

typedef struct runner_args runner_args;
struct runner_args{
    size_t starting_index;
    size_t num_operations;
};

void* run_operations(void* args){
    runner_args args_unpacked = *(runner_args *)args;

    for(size_t i = 0; i < args_unpacked.num_operations;i++){
        int argument = operation_arg_list[args_unpacked.starting_index + i];
        switch (operation_list[args_unpacked.starting_index + i]) {
            // I wonder if I have to be malloc'ing in here
            case TREE_SEARCH:
                tree_search(head, argument);
                break;
            case TREE_INSERT:
                tree_insert(head, argument);
                break;
            case TREE_DELETE:
                tree_delete(head, argument);
                break;
        }
    }

    return NULL;
}

int main(){
    rng64_randomize();
    rng64_intrange_spec nums;
    rng64_intrange_spec functions;
    rng64_set_intrange(&nums, INT_MIN, INT_MAX);
    rng64_set_intrange(&functions, 0, 2);
    
    head = init_node();

    for(size_t i = 0; i < NUM_VALUES; i++){
        int random_number = (int) rng64_intrange(&nums);
        tree_insert(head, random_number);
    }
    
    for(size_t i = 0; i < NUM_OPERATIONS; i++){
        int random_arg = (int) rng64_intrange(&nums);
        int random_operation = (int) rng64_intrange(&functions);
        
        operation_arg_list[i] = random_arg;
        operation_list[i] = random_operation;
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
