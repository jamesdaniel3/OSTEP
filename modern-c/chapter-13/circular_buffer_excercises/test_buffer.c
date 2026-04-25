#include <stdlib.h>
#include <stdio.h>
#include "circular_buffer.h"

#define TABLE_SIZE 10

void test_shrink_resize_no_wrap(){
    // Shrink an array where all data fits in smaller array without moving
    double* test_table = malloc(TABLE_SIZE*sizeof(double));

    for (size_t index = 0; index < 3; index++){
        test_table[index] = index + 1;
    }

    circular test_data = {
        .start = 0,
        .len = 3,
        .cap = 10,
        .tab = test_table,
    };

    print_buffer_info(&test_data);
    circular_resize(&test_data, 5);
    print_buffer_info(&test_data);
    printf("\n");
    free(test_data.tab);
}

void test_shrink_resize_no_wrap_becomes_wrap(){
    // Shrink an array where previously all the data fit without wrapping and now some of the data has to wrap
    double* test_table = malloc(TABLE_SIZE*sizeof(double));

    for (size_t index = 6; index < TABLE_SIZE; index++){
        test_table[index] = index + 1;
    }

    circular test_data = {
        .start = 6,
        .len = 4,
        .cap = 10,
        .tab = test_table,
    };

    print_buffer_info(&test_data);
    circular_resize(&test_data, 7);
    print_buffer_info(&test_data);
    printf("\n");
    free(test_data.tab);
}

void test_shrink_resize_wrap_wraps_more(){
    // Shrink an array where the data wrapped and now it still has to wrap 
    double* test_table = malloc(TABLE_SIZE*sizeof(double));

    for (size_t index = 0; index < 2; index++){
        test_table[index] = index + 1;
    }

    for (size_t index = 6; index < TABLE_SIZE; index++){
        test_table[index] = index + 1;
    }

    circular test_data = {
        .start = 6,
        .len = 6,
        .cap = 10,
        .tab = test_table,
    };

    print_buffer_info(&test_data);
    circular_resize(&test_data, 8);
    print_buffer_info(&test_data);
    printf("\n");
    free(test_data.tab);
}

void test_shrink_resize_wrap_becomes_no_wrap(){
    // Shrink an array that wrapped and must shift to start at 0

    double* test_table = malloc(TABLE_SIZE*sizeof(double));

    for (size_t index = 0; index < 2; index++){
        test_table[index] = index + 1;
    }

    for (size_t index = 8; index < TABLE_SIZE; index++){
        test_table[index] = index + 1;
    }

    circular test_data = {
        .start = 8,
        .len = 4,
        .cap = 10,
        .tab = test_table,
    };

    print_buffer_info(&test_data);
    circular_resize(&test_data, 6);
    print_buffer_info(&test_data);
    printf("\n");
    free(test_data.tab);
}


int main(){
    test_shrink_resize_no_wrap();
    test_shrink_resize_no_wrap_becomes_wrap();
    test_shrink_resize_wrap_wraps_more();
    test_shrink_resize_wrap_becomes_no_wrap();
}