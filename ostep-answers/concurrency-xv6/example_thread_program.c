#include "types.h"
#include "user.h"

void test_func(void* arg1, void* arg2){
    int first_arg = *(int *)arg1;
    int second_arg = *(int *)arg2;
    
    printf(1, "hello from child with args %d and %d\n", first_arg, second_arg);
    exit();
}

int main(){
    printf(1, "hello from parent\n");
    
    int* first_arg = malloc(sizeof(int));
    int* second_arg = malloc(sizeof(int));
    if (!first_arg || !second_arg){
        printf(1, "Failed Malloc!\n");
        exit();
    }

    *first_arg = 3;
    *second_arg = 6;

    thread_create(test_func, (void *)first_arg, (void *)second_arg);
    int child_pid = thread_join();
    printf(1, "parent has seen child process %d, goodbye!\n", child_pid);
    exit();
}
