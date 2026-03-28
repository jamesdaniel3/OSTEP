#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void merge(double* nums, double* tempArray, int left, int right){
    int middle = left + (right - left) / 2;

    int currentLeft = left;
    int currentRight = middle + 1;
    int currentInsertIndex = left;

    while (currentLeft <= middle && currentRight <= right) {
        if (nums[currentLeft] <= nums[currentRight]){
            tempArray[currentInsertIndex] = nums[currentLeft];
            currentLeft++;
        }
        else {
            tempArray[currentInsertIndex] = nums[currentRight];
            currentRight++;
        }
        currentInsertIndex++;
    }

    while (currentLeft <= middle) {
        tempArray[currentInsertIndex] = nums[currentLeft];
        currentLeft++;
        currentInsertIndex++;
    }

    while (currentRight <= right) {
        tempArray[currentInsertIndex] = nums[currentRight];
        currentRight++;
        currentInsertIndex++;
    }

    for (int i = left; i <= right; i++){
        nums[i] = tempArray[i];
    }

    return;
}

// can't just use mergesort because stdlib has a function named mergesort
void my_mergesort(double* nums, double* tempArray, int left, int right){
    if (left < right) {  // there are at least two elements 
        int middle = left + (right - left) / 2;

        my_mergesort(nums, tempArray, left, middle);
        my_mergesort(nums,tempArray, middle + 1, right);

        merge(nums, tempArray, left, right);
    }
}