#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "mergesort-extended.h"

void merge(void* base, void* temp_array, size_t element_size, size_t left, size_t right, compare_function* comparator){
    size_t middle = left + (right - left) / 2;

    size_t current_left = left;
    size_t current_right = middle + 1;
    size_t current_insert_index = left;

    while (current_left <= middle && current_right <= right) {
        void * current_left_ptr = (char*)base + current_left * element_size;
        void * current_right_ptr = (char*)base + current_right * element_size;
        void * current_insert_pointer = (char*)temp_array + current_insert_index * element_size;

        if (comparator(current_left_ptr, current_right_ptr) <= 0){
            memcpy(current_insert_pointer, current_left_ptr, element_size);
            current_left++;
        }
        else {
            memcpy(current_insert_pointer, current_right_ptr, element_size);
            current_right++;
        }
        current_insert_index++;
    }

    while (current_left <= middle) {
        void * current_left_ptr = (char*)base + current_left * element_size;
        void * current_insert_pointer = (char*)temp_array + current_insert_index * element_size;

        memcpy(current_insert_pointer, current_left_ptr, element_size);
        current_left++;
        current_insert_index++;
    }

    while (current_right <= right) {
        void * current_right_ptr = (char*)base + current_right * element_size;
        void * current_insert_pointer = (char*)temp_array + current_insert_index * element_size;

        memcpy(current_insert_pointer, current_right_ptr, element_size);
        current_right++;
        current_insert_index++;
    }

    if (right < left) {
        return;
    }

    memcpy(
        base + left * element_size, 
        temp_array + left * element_size, 
        ((right - left) + 1) * element_size
    );
}

// can't just use mergesort because stdlib has a function named mergesort
void my_mergesort(void* base, void* temp_array, size_t element_size, size_t left, size_t right, compare_function* comparator){
    if (left < right) {  // there are at least two elements 
        size_t middle = left + (right - left) / 2;

        my_mergesort(base, temp_array, element_size, left, middle, comparator);
        my_mergesort(base, temp_array, element_size,  middle + 1, right, comparator);

        merge(base, temp_array, element_size, left, right, comparator);
    }
}

void mergesort_generic(void* base, size_t element_size, size_t num_elements, compare_function* comparator){
    void * temp_array = malloc(num_elements * element_size);

    my_mergesort(base, temp_array, element_size, 0, num_elements - 1, comparator);
}