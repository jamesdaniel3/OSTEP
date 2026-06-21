/*
    Challenge 21

Can you implement a parallel algorithm for sorting using two threads that builds on your merge sort implementation (challeges 1 and 11)?

That is, a merge sort cuts the input array in half, sorts each half in its own thread, and then merges the two halves sequentially as before. Use different sequential 
sorting algorithms as a base inside each of the two threads. 

Can you generalize this parallel sorting to P threads, where P = 2^k for k = 1, 2, 3, 4, where k is given on the command line?

Can you measure the speedup you obtained as a result of your parallelization? Does it match the number of cores that your test platform has?
*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>            // Apparently mac does not have threads.h 
#include "rng64.h"

#define BILLION 1000000000.0
#define NUMS_LENGTH 10000000
#define DEFAULT_NUM_THREADS 2

double* temp_array = NULL;

typedef struct sort_func_args sort_func_args;
struct sort_func_args {
    double * nums;
    int left;
    int right;
};

void merge(double* nums, int left, int right){
    int middle = left + (right - left) / 2;

    int current_left = left;
    int current_right = middle + 1;
    int current_insert_index = left;

    while (current_left <= middle && current_right <= right) {
        if (nums[current_left] <= nums[current_right]){
            temp_array[current_insert_index] = nums[current_left];
            current_left++;
        }
        else {
            temp_array[current_insert_index] = nums[current_right];
            current_right++;
        }
        current_insert_index++;
    }

    while (current_left <= middle) {
        temp_array[current_insert_index] = nums[current_left];
        current_left++;
        current_insert_index++;
    }

    while (current_right <= right) {
        temp_array[current_insert_index] = nums[current_right];
        current_right++;
        current_insert_index++;
    }

    for (int i = left; i <= right; i++){
        nums[i] = temp_array[i];
    }

    return;
}

void* left_sort_func(void * arg){
    // could be any sequential sorting algoirthm
    sort_func_args *args = (sort_func_args *)arg;

    if (args->left >= args->right) {
        return NULL;
    }

    int middle = args->left + (args->right - args->left) / 2;

    sort_func_args left_args = {
        .nums = args->nums,
        .left = args->left,
        .right = middle
    };
    sort_func_args right_args = {
        .nums = args->nums,
        .left = middle + 1,
        .right = args->right
    };

    // we don't have to malloc vars here because the vars defined in the function will never be cleaned up before the child function returns
    left_sort_func(&left_args);
    left_sort_func(&right_args);

    merge(args->nums, args->left, args->right);

    // free(arg);
    return NULL;
}

void* right_sort_func(void * arg){
    // could be any sequential sorting algoirthm
    sort_func_args *args = (sort_func_args *)arg;

    if (args->left >= args->right) {
        return NULL;
    }

    int middle = args->left + (args->right - args->left) / 2;

    sort_func_args left_args = {
        .nums = args->nums,
        .left = args->left,
        .right = middle
    };
    sort_func_args right_args = {
        .nums = args->nums,
        .left = middle + 1,
        .right = args->right
    };

    // we don't have to malloc vars here because the vars defined in the function will never be cleaned up before the child function returns
    right_sort_func(&left_args);
    right_sort_func(&right_args);

    merge(args->nums, args->left, args->right);

    // free(arg);
    return NULL;
}


typedef struct dispatch_args dispatch_args;
struct dispatch_args {
    double * nums;
    int left, right;
    size_t open_threads_remaining;
    bool is_left_tree;
};

// forward declaration so sort_dispatch can call sort() before it's defined
void sort(double* nums, int left, int right, size_t open_threads_remaining, bool is_left_tree);

void* sort_dispatch(void* arg) {
    dispatch_args *a = (dispatch_args*)arg;
    sort(a->nums, a->left, a->right, a->open_threads_remaining, a->is_left_tree);
    free(a);
    return NULL;
}

void sort(double* nums, int left, int right, size_t open_threads_remaining, bool is_left_tree) {
    if (left >= right) {
        return;
    }

    if (open_threads_remaining <= 1) {
        sort_func_args args = { .nums = nums, .left = left, .right = right };
        if (is_left_tree) {
            left_sort_func(&args);
        } else {
            right_sort_func(&args);
        }
        return;
    }

    int middle = left + (right - left) / 2;
    size_t half = open_threads_remaining / 2;

    pthread_t dispatcher;
    dispatch_args *rargs = malloc(sizeof(dispatch_args));
    *rargs = (dispatch_args){
        .nums = nums,
        .left = middle + 1,
        .right = right,
        .open_threads_remaining = half,
        .is_left_tree = false
    };
    pthread_create(&dispatcher, NULL, sort_dispatch, rargs);

    sort(nums, left, middle, half, true);

    pthread_join(dispatcher, NULL);
    merge(nums, left, right);
}

double * generate_test_data(size_t list_size) {
    rng64_randomize();
    rng64_intrange_spec  r;
    rng64_set_intrange(&r, 1, 100);

    double * data = calloc(list_size, sizeof(double));
    size_t current_index = 0;

    while (current_index < list_size) {
        data[current_index] = rng64_one();
        current_index++;
    }

    return data;
}

double time_base_sort(double* nums) {
    struct timespec start, end;
    double time_spent;

    sort_func_args args = {
        .nums = nums,
        .left = 0,
        .right = NUMS_LENGTH - 1
    };

    clock_gettime(CLOCK_MONOTONIC, &start);
    left_sort_func(&args);
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time difference
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

double time_sort(double* nums, size_t num_threads) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    sort(nums, 0, NUMS_LENGTH - 1, num_threads, true);
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

bool is_data_sorted(double * nums, size_t len) {
    for(size_t i = 1; i < len; i++){
        if (nums[i] < nums[i - 1]) {
            return false;
        }
    }

    return true;
}


int main(){
    size_t max_threads_arr[7] = {0, 2, 4, 8, 16, 32, 64};
    double results[7] = {};

    temp_array = calloc(NUMS_LENGTH, sizeof(double));
    for (size_t i = 0; i < 7; i++){
        size_t current_max_threads = max_threads_arr[i];

        double * nums = generate_test_data(NUMS_LENGTH);
        double time_spent = i == 0 ? time_base_sort(nums) : time_sort(nums, current_max_threads);
        results[i] = time_spent;

        if (!is_data_sorted(nums, NUMS_LENGTH)){
            printf("Sort Failed!\n");
            exit(1);
        }

        free(nums);
    }
    free(temp_array);

    for (size_t i = 0; i < 6; i++){
        printf("Time to sort data with %zu threads: %f\n", max_threads_arr[i], results[i]);
    }

    return 0;
}