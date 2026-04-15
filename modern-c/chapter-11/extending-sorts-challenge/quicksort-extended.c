#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort-extended.h"

// Note: quick sort imeplementations from earlier chapters do not work for many reasons 

int hoare_partition_generic(void* vals, size_t element_size, int left, int right, compare_function* comparator){
    char pivot[element_size];
    memcpy(pivot, (char*)vals + ((left + right) / 2) * element_size, element_size);

    while (1) {
        void* left_ptr = (char*)vals + left * element_size;
        void* right_ptr = (char*)vals + right * element_size;

        while (comparator(left_ptr, pivot) < 0) {
            left++;
            left_ptr = (char*)vals + left * element_size;
        }

        while (comparator(right_ptr, pivot) > 0) {
            right--;
            right_ptr = (char*)vals + right * element_size;
        }

        if (left >= right) return right;

        char temp[element_size];
        memcpy(temp, left_ptr, element_size);
        memcpy(left_ptr, right_ptr, element_size);
        memcpy(right_ptr, temp, element_size);

        left++;
        right--;
    }
}

void __quicksort_generic(void* base, size_t element_size, int left, int right, compare_function* comparator){
    if (left >= right) {
        return;
    }

    int pivot = hoare_partition_generic(base, element_size, left, right, comparator);

    __quicksort_generic(base, element_size, left, pivot, comparator);
    __quicksort_generic(base, element_size, pivot + 1, right, comparator);
}

void quicksort_generic(void* base, size_t element_size, size_t num_elements, compare_function* comparator){
    __quicksort_generic(base, element_size, 0, (int)num_elements - 1, comparator);
}