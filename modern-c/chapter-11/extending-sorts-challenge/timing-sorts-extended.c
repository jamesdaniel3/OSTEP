/*
CHALLENGE 14

Can you extend your sorting algorithms (challenge 3.1) to other sort keys?
Can you condense your functions for different sort keys to functions that have the same signature as qsort
--that is, recieve generic pointers to data, size information, and a comparison function as parameters?

Can you extend the performance comparison of your sorting algorithms (challenge 8.6) to the C library function qsort?
*/

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "student.h"
#include "quicksort-extended.h"
#include "mergesort-extended.h"

#define BILLION 1000000000.0

enum sizes { FIRST_SIZE = 100, SECOND_SIZE = 10000, THIRD_SIZE = 100000 };
size_t sizes_array[3] = {FIRST_SIZE, SECOND_SIZE, THIRD_SIZE};


double time_quicksort(student* data, int num_elements) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    quicksort_generic(data, sizeof(student), num_elements, compare_students_by_name);
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time difference
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

double time_c_quicksort(student* data, int num_elements) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    qsort(data, num_elements, sizeof(student), compare_students_by_name);
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time difference
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

double time_mergesort(student* data, int num_elements) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    mergesort_generic(data, sizeof(student), num_elements, compare_students_by_name);
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

int main() {
    srand(time(NULL)); // get random seed 
    student data[THIRD_SIZE];

    for(size_t current_index = 0; current_index < 3; current_index++){
        int current_size = sizes_array[current_index];
        generate_random_students_list(data, current_size);
        double time_spent_current = time_quicksort(data, current_size);
        printf("Quicksort took %f seconds to sort a list of size %d.\n", time_spent_current, current_size);
    }

    for(size_t current_index = 0; current_index < 3; current_index++){
        int current_size = sizes_array[current_index];
        generate_random_students_list(data, current_size);
        double time_spent_current = time_c_quicksort(data, current_size);
        printf("System Implementation of quicksort took %f seconds to sort a list of size %d.\n", time_spent_current, current_size);
    }

    for(size_t current_index = 0; current_index < 3; current_index++){
        int current_size = sizes_array[current_index];
        generate_random_students_list(data, current_size);
        double time_spent_current = time_mergesort(data, current_size);
        printf("Mergesort took %f seconds to sort a list of size %d.\n", time_spent_current, current_size);
    }
}