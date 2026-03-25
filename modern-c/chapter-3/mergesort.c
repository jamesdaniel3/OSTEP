#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/*
There is something called a Variable Length Array (VLA), which is what is used when you do something like int Arr [var]; and people hate them. 
I don't really understand why yet and I am trying to figure it out.

https://www.reddit.com/r/C_Programming/comments/1k9pj3l/whats_the_use_of_vlas/
https://www.reddit.com/r/C_Programming/comments/hf00ak/why_vlas_are_bad/
https://stackoverflow.com/questions/31199566/whats-the-difference-between-a-vla-and-dynamic-memory-allocation-via-malloc
https://c-faq.com/aryptr/index.html

Some differences between a VLA and Malloc are:
- VLAs create arrays while malloc creates pointer to memory, this means that:
    - VLAs are generally stored on the stack while malloc'd things are always stored on the heap 
    - VLAs have a fixed size while malloc's can be adjusted 
    - VLAs can be freed automatically while malloc requires manual freeing 
*/


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


void sort(double* nums, double* tempArray, int left, int right){
    if (left < right) {  // there are at least two elements 
        int middle = left + (right - left) / 2;

        sort(nums, tempArray, left, middle);
        sort(nums,tempArray, middle + 1, right);

        merge(nums, tempArray, left, right);
    }
}

bool checkSort(double* nums, size_t numsSize){
    if (numsSize < 2) return true;

    for(size_t currentIndex = 0; currentIndex < numsSize - 1; currentIndex++) {
        if (nums[currentIndex] > nums[currentIndex + 1]) {
            return false;
        }
    }

    return true;
}


int main(int argc, char** argv){
    if (argc < 2) {
        printf("There is nothing to sort!\n");
        return 0;
    }

    int numsLength = argc - 1;
    double nums[numsLength];
    char* end;

    for (int i = 0; i < numsLength; i++) {
        nums[i] = strtod(argv[i + 1], &end);

        if (end == argv[i + 1]) {
            printf("Invalid number: %s\n", argv[i + 1]);
            return 1;
        }
    }

    double* tempArray = malloc(numsLength * sizeof(double));
    if (!tempArray) {
        perror("malloc failed");
        return 1;
    }

    sort(nums, tempArray, 0, numsLength - 1);

    free(tempArray);

    bool isSorted = checkSort(nums, numsLength);

    if (isSorted){
        printf("The list is sorted! The sorted order is:\n");
        for (int i = 0; i < numsLength; i++) {
            printf("%lf ", nums[i]);
        }
        printf("\n");
    }
    else {
        printf("The list is not sorted :( The order is:\n");
        for (int i = 0; i < numsLength; i++) {
            printf("%lf ", nums[i]);
        }
        printf("\n");
    }

    return 0;
}