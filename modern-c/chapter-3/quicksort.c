#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*
One of many explanations as to why Hoare's partitioning method is better than Lomuto's can be found here: 
https://cs.stackexchange.com/questions/11458/quicksort-partitioning-hoare-vs-lomuto
*/

int hoarePartition(double* nums, int left, int right){
    // as best as I can tell hoare's is better because it handles duplicates better than lomuto
    // it also might perform better in the case of already sorted lists 
    double pivotValue = nums[(left + right) / 2];

    while (left < right) {
        if (nums[right] > pivotValue) {
            right--;
            continue;
        }

        if (nums[left] < pivotValue) {
            left++;
            continue;
        }

        double temp = nums[left];
        nums[left] = nums[right];
        nums[right] = temp;

        left++;
        right--;
    }

    return right;
}

int lomutoPartition(double* nums, int left, int right) {
    double pivotValue = nums[right];

    for (int currentPosition = left + 1; currentPosition < right; currentPosition++){
        double currentValue = nums[currentPosition];

        if (pivotValue >= currentValue) { // what happens if this is just >?
            double temp = currentValue;
            nums[currentPosition] = nums[left];
            nums[left] = temp;
            left++;
        }
    }

    double temp = nums[right];
    nums[right] = nums[left];
    nums[left] = temp;

    return left;
}

void sort(double* nums, int left, int right){
    if (right - left + 1 <= 1) {
        return;
    }

    int pivot = hoarePartition(nums, left, right);

    sort(nums, left, pivot - 1);
    sort(nums, pivot + 1, right);

}

bool checkSort(double* nums, size_t numsSize){
    for(size_t currentIndex = 0; currentIndex < numsSize - 1; currentIndex++) {
        if (nums[currentIndex] > nums[currentIndex + 1]) {
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("There is nothing to sort!\n");
        return 0;
    }

    double nums[argc - 1];
    size_t numsSize = argc - 1;
    char* end;

    for (int i = 0; i < argc - 1; i++) {
        nums[i] = strtod(argv[i + 1], &end);
    }

    sort(nums, 0, numsSize - 1);

    bool listSorted = checkSort(nums, numsSize);

    if (listSorted) {
        printf("The list, in sorted order, is:\n");
    } else {
        printf("The list is not sorted. The contents of the list are:\n");
    }

    for(size_t currentIndex = 0; currentIndex < numsSize; currentIndex++) {
        printf("%lf ", nums[currentIndex]);
    }

    printf("\n");
}
