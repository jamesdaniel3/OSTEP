/*
CHALLENGE 14

Can you extend your sorting algorithms (challenge 3.1) to other sort keys?
Can you condense your functions for different sort keys to functions that have the same signature as qsort
--that is, recieve generic pointers to data, size information, and a comparison function as parameters?

typedef int compare_function(void const*, void const*);
void qsort(void* base, size_t n, size_t size, compare_function* compar);

Can you extend the performance comparison of your sorting algorithms (challenge 8.6) to the C library function qsort?
*/

#include <stdbool.h>
#include <stdio.h>
#include "student.h"
#include "quicksort-extended.h"

#define NUM_STUDENTS 10

typedef int compare_function(void const*, void const*);

bool sort_check_generic(void* base, size_t element_size, size_t num_elements, compare_function* comparator){
    size_t current_element = 0;
    while (current_element < num_elements - 1) {
        void* first_element_ptr = (char *)base + current_element * element_size;
        void* second_element_ptr = (char *)base + (current_element + 1) * element_size;
        if (comparator(first_element_ptr, second_element_ptr) > 0){
            return false;
        }
        current_element++;
    }
    return true;
}


int main(){
    student* test_students = generate_random_students_list(NUM_STUDENTS);
    quicksort_generic(test_students, sizeof(student), NUM_STUDENTS, compare_students_by_name);

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