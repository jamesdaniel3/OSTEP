#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "quicksort.h"
#include "mergesort.h"

#define BILLION 1000000000.0

enum sizes { FIRST_SIZE = 100, SECOND_SIZE = 10000, THIRD_SIZE = 100000 };
size_t sizes_array[3] = {FIRST_SIZE, SECOND_SIZE, THIRD_SIZE};

void randomize_list(double* data, size_t size) {
    for (size_t i = 0; i < size; i++){
        double random_value = rand();
        data[i] = random_value;
    }
}

double time_quicksort(double* data, int data_size) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    quicksort(data, 0, data_size - 1);
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time difference
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

double time_mergesort(double* data, int data_size) {
    struct timespec start, end;
    double time_spent;

    double* third_temp_array = malloc(data_size * sizeof(double));
    if (!third_temp_array) {
        perror("malloc failed");
        return 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    my_mergesort(data, third_temp_array, 0, data_size - 1);
    clock_gettime(CLOCK_MONOTONIC, &end);

    free(third_temp_array);
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

int main() {
    srand(time(NULL)); // get random seed 
    double data[THIRD_SIZE];

    for(size_t current_index = 0; current_index < 3; current_index++){
        int current_size = sizes_array[current_index];
        randomize_list(data, current_size);
        double time_spent_current = time_quicksort(data, current_size);
        printf("Quicksort took %f seconds to sort a list of size %d.\n", time_spent_current, current_size);
    }

    for(size_t current_index = 0; current_index < 3; current_index++){
        int current_size = sizes_array[current_index];
        randomize_list(data, current_size);
        double time_spent_current = time_mergesort(data, current_size);
        printf("Mergesort took %f seconds to sort a list of size %d.\n", time_spent_current, current_size);
    }
}