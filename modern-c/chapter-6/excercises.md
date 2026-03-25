## Excercise 1

#### Use `memchr` and `memcmp` to implement a bounds-checking version of strcmp

```c
#include <string.h>
#include <stdio.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(){
    const char FIRST_WORD[6] = "james";
    const char SECOND_WORD[4] = "jay";

    int firstLength = sizeof(FIRST_WORD) / sizeof(char);
    int secondLength = sizeof(SECOND_WORD) / sizeof(char);

    if(!memchr(FIRST_WORD, '\0', firstLength) || !memchr(SECOND_WORD, '\0', secondLength)) {
        printf("At least one of your strings is not null terminated.\n");
        return -1;
    }

    int result = memcmp(FIRST_WORD, SECOND_WORD, MIN(firstLength, secondLength));

    printf("The result is %d\n", result);

    return result;
}
```

## Excercise 2

#### Write a function timespec_diff that conputes the difference between two timespec values

```c
#include <time.h>
#include <stdio.h>

struct timespec timespec_diff(struct timespec first_time, struct timespec second_time){
    struct timespec result = {
        .tv_sec = (second_time.tv_sec - first_time.tv_sec),
        .tv_nsec = (second_time.tv_nsec - first_time.tv_nsec)
    };

    return result;
}

int main() {
    const struct timespec FIRST_TIME = {
        .tv_sec = 1,
        .tv_nsec = 1,
    };
    const struct timespec SECOND_TIME = {
        .tv_sec = 43,
        .tv_nsec = 200,
    };

    struct timespec time_difference = timespec_diff(FIRST_TIME, SECOND_TIME);
    int seconds_difference = time_difference.tv_sec;
    long nano_seconds_difference = time_difference.tv_nsec;
    double converted_nano_seconds = (double) nano_seconds_difference / 1000000000;
    double result = seconds_difference + converted_nano_seconds;

    printf("The difference between your first time and your second time is %.9f seconds.\n", result);
}
```

## Excercise 3

#### Create six different structure types for each possibility to order three fields inside a structure: one unsigned char, one unsigned, and one unsigned long long. Print the sizes of these six structures; they should be significantly different. Compute the minimal size as the sum of the sizes of each member. Does any of your structure have this size? Which of your structures comes closest to that ideal size?

```c

#include <stdio.h>

struct first_combo {
    unsigned char character_value; 
    unsigned integer_value; 
    unsigned long long long_value;  
};

struct second_combo {
    unsigned integer_value;  
    unsigned char character_value; 
    unsigned long long long_value; 
};

struct third_combo {
    unsigned integer_value;  
    unsigned long long long_value; 
    unsigned char character_value; 
};

struct fourth_combo {
    unsigned long long long_value;
    unsigned char character_value; 
    unsigned integer_value;  
};

struct fifth_combo {
    unsigned long long long_value; 
    unsigned integer_value;  
    unsigned char character_value;
};

struct sixth_combo {
    unsigned char character_value; 
    unsigned long long long_value; 
    unsigned integer_value;  
};


int main() {
    printf("The size of unsigned char is: %zu\n", sizeof(unsigned char)); // 1 
    printf("The size of unsigned long long is: %zu\n", sizeof(unsigned long long)); // 8
    printf("The size of unsigned is: %zu\n\n", sizeof(unsigned)); // 4
    // minimal size (no padding) is 13 

    printf("The size of struct 1 is: %zu\n", sizeof(struct first_combo)); // 16
    printf("The size of struct 2 is: %zu\n", sizeof(struct second_combo)); // 16
    printf("The size of struct 3 is: %zu\n", sizeof(struct third_combo)); // 24
    printf("The size of struct 4 is: %zu\n", sizeof(struct fourth_combo)); // 16
    printf("The size of struct 5 is: %zu\n", sizeof(struct fifth_combo)); // 16
    printf("The size of struct 6 is: %zu\n", sizeof(struct sixth_combo)); // 24
}
```

None of the structures have the minial size, but many are tied for the closest. 