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


double time_quicksort(student* data, int data_size) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    quicksort_generic(data, sizeof(student), data_size, compare_students_by_name);
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate time difference
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    return time_spent;
}

double time_mergesort(student* data, int data_size) {
    struct timespec start, end;
    double time_spent;

    clock_gettime(CLOCK_MONOTONIC, &start);
    mergesort_generic(data, sizeof(student), data_size, compare_students_by_name);
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
        double time_spent_current = time_mergesort(data, current_size);
        printf("Mergesort took %f seconds to sort a list of size %d.\n", time_spent_current, current_size);
    }
}

/*


int main(){
    student* test_students = generate_random_students_list(NUM_STUDENTS);
    // quicksort_generic(test_students, sizeof(student), NUM_STUDENTS, compare_students_by_name);
    mergesort_generic(test_students, sizeof(student), NUM_STUDENTS, compare_students_by_name);

    bool result = sort_check_generic(test_students, sizeof(student), NUM_STUDENTS, compare_students_by_name);

    if (result == true) {
        printf("Listed sucessfully sorted!\n");
        return 0;
    }

    printf("Failed to sort!\n");

    size_t current_index = 0;
    while(current_index < NUM_STUDENTS) {
        student current_student = test_students[current_index];
        printf("Student %zu:\n", current_index + 1);
        printf("-Name: %s\n", current_student.name);
        printf("-Grade: %s\n", current_student.grade);
        printf("-GPA: %f\n", current_student.gpa);

        current_index++;
    }
}
    */