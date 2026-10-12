#include "types.h"
#include "user.h"

#define MILLION 1000000

int value;
lock_t* lock;

void test_func(void* arg1, void* arg2){
    lock_acquire(lock);
    int temp = value;
    sleep(1);          // force a process switch in critical section
    temp += MILLION;
    value = temp;
    lock_release(lock);

    exit();
}

int main(){
    int* first_arg = malloc(sizeof(int));
    int* second_arg = malloc(sizeof(int));
    if (!first_arg || !second_arg){
        printf(1, "Failed Malloc!\n");
        exit();
    }

    value = 0;
    lock = malloc(sizeof(lock_t));
    if (!lock){
        printf(1, "Failed Malloc!\n");
        exit();
    }
    lock_init(lock);

    for(int i = 0; i < 5; i++){
        thread_create(test_func, first_arg, second_arg);
    }

    for(int i = 0; i < 5; i++){
        thread_join();
    }

    if (value != 5 * MILLION){
        printf(1, "Failed! Value is %d.\n", value);
    }
    else{
        printf(1, "Passed!\n");
    }
    exit();
}
