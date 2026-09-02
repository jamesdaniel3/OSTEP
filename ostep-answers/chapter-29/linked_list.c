/*
Build a version of a linked list that uses hand-over-hand locking
[MS04], as cited in the chapter. You should read the paper first
to understand how it works, and then implement it. Measure its
performance. When does a hand-over-hand list work better than a
standard list as shown in the chapter?

Hand-over hand locking should improve the performance of the linked list when
there are mutiple threads doing concurrent operations at different places in 
the list. With the functions we have defined below; it would help us if we had
multiple threads searching the list and/or updating it. 

Results from this test:

Time to search for 5000 values with 1 threads: 48.1014983010
Time to search for 5000 values with 2 threads: 25.3336434560
Time to search for 5000 values with 3 threads: 16.7240162690
Time to search for 5000 values with 4 threads: 14.0512039060
Time to search for 5000 values with 5 threads: 15.2014167080
 */

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "rng64.h"

#define LIST_SIZE 100000
#define NUM_SEARCHES 5000
#define BILLION 1000000000.0
#define MAX_THREAD_COUNT 5

typedef struct list_node list_node;
struct list_node {
    int val;
    list_node* next;
    pthread_mutex_t lock;
};
list_node* head;

int nums_to_search[NUM_SEARCHES];

void add_node_to_head(int val) {
    list_node* new_node = malloc(sizeof(list_node));
    assert(new_node != NULL);

    int rc = pthread_mutex_init(&new_node->lock, NULL); // should check rc
    assert(rc == 0);

    new_node->val = val;
    new_node->next = head;
    
    pthread_mutex_lock(&head->lock);
    list_node* previous_head = head;
    head = new_node;
    pthread_mutex_unlock(&previous_head->lock);
}

int search(int desired_val){
    pthread_mutex_lock(&head->lock);
    list_node* current_node = head;
    
    while (current_node->val != desired_val){
        if (current_node->next == NULL) {
            pthread_mutex_unlock(&current_node->lock);
            return -1;
        }

        list_node* previous_node = current_node;
        current_node = current_node->next;
        pthread_mutex_lock(&current_node->lock);
        pthread_mutex_unlock(&previous_node->lock);
    }

    pthread_mutex_unlock(&current_node->lock);
    return 0;
}

typedef struct search_args search_args;
struct search_args{
    int* list_start;
    size_t list_size;
};

typedef struct search_results search_results;
struct search_results{
    size_t present;
    size_t absent;
};

void* run_searches(void* args){
    search_args args_unpacked = *(search_args *)args;
    size_t total_present = 0;
    size_t total_absent = 0;

    for(size_t i = 0; i < args_unpacked.list_size; i++){
        int result = search(*(args_unpacked.list_start + i));
        
        if(result == 0){
            total_present++;
        }
        else{
            total_absent++;
        }
    }

    search_results* ret = malloc(sizeof(search_results));
    assert(ret != NULL);
    ret->present = total_present;
    ret->absent = total_absent;

    return (void *)ret;
}

int main(){
    head = malloc(sizeof(list_node));
    assert(head != NULL);
    head->next = NULL;
    head-> val = 0;
    pthread_mutex_init(&head->lock, NULL);

    // fill the list
    rng64_randomize();
    rng64_intrange_spec  spec;
    rng64_set_intrange(&spec, INT_MIN, INT_MAX);
    
    for(size_t i = 0; i < LIST_SIZE; i++){
        int random_number = (int) rng64_intrange(&spec);
        add_node_to_head(random_number);
    }
    
    for(size_t i = 0; i < NUM_SEARCHES; i++){
        int random_number = (int) rng64_intrange(&spec);
        nums_to_search[i] = random_number;
    }

    struct timespec start, end;

    for(size_t num_threads = 1; num_threads <= MAX_THREAD_COUNT; num_threads++){
        size_t count_per_thread = NUM_SEARCHES / num_threads;
        size_t remainder = NUM_SEARCHES % num_threads;    

        pthread_t threads[MAX_THREAD_COUNT];
        search_args args[MAX_THREAD_COUNT];
        search_results results[MAX_THREAD_COUNT];
        clock_gettime(CLOCK_MONOTONIC, &start);
        for(size_t j = 0; j < num_threads; j++){
            args[j].list_start = nums_to_search + (count_per_thread * j);
            args[j].list_size = count_per_thread;

            int rc = pthread_create(threads + j, NULL, run_searches, args + j);
            assert(rc == 0);
        }

        for(size_t j = 0; j < num_threads; j++){
            void* current_result;
            pthread_join(threads[j], &current_result);
            
            search_results *result = current_result;
            results[j] = *result;
            free(result);
        }
    
        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

        printf("Time to search for %d values with %ld threads: %.10f \n", NUM_SEARCHES, num_threads, time_spent);    

        
        size_t total_present = 0;
        size_t total_absent = 0;
        for (size_t i = 0; i < num_threads; i++){
            total_present += results[i].present;
            total_absent += results[i].absent;
        }

        assert(NUM_SEARCHES == (total_present + total_absent + remainder));

        for (size_t i = 0; i < num_threads; i++){
            results[i].present = 0;
            results[i].absent = 0;
        }   
    }
}
