#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define PAGE_SIZE 4096 // guessing that test systems will have 4KB pages
#define BILLION 1000000000

int main(int argc, char* argv[]){
    struct timespec start, end;
    long num_pages = strtol(argv[1], NULL, 10);
    long num_trials = strtol(argv[2], NULL, 10);

    int elements_per_page = PAGE_SIZE / sizeof(int);
    int arr_len = num_pages * elements_per_page;
    int * arr = calloc(arr_len, sizeof(int));

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(long i = 0; i < num_trials; i++){
        for(int j = 0; j < arr_len; j += elements_per_page) {
            arr[j] += 1;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    double time_per_access = time_spent / (num_trials * num_pages);

    printf("%ld Pages; %ld Trials: %.15f Nanoseconds Per Accesss\n", num_pages, num_trials, time_per_access * BILLION);

    return 0;
}