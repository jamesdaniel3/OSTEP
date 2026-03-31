## Excercise 1

#### Write a function that receives pointers to three objects and that shifts the values of these objects cyclically

```c
void cycle_ints(int* first_num_loc, int* second_num_loc, int* third_num_loc) {
    int temp = *third_num_loc;
    *third_num_loc = *second_num_loc;
    *second_num_loc = *first_num_loc;
    *first_num_loc = temp;
    return;
}
```