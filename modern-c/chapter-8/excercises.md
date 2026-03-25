## Excercise 1

#### Write a function `my_strt0d` that implements the functionality of `strtod` for decima floating point constants

```c
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

double my_strtod(const char *num_as_string, char** first_nonviable_character){
    double result = 0;
    size_t current_index = 0;
    bool is_negative = false;
    bool is_after_decimal = false;

    if (num_as_string[current_index] == '-') {
        is_negative = true;
        current_index++;
    }

    while (num_as_string[current_index] != '\0') {
        if (num_as_string[current_index] == '.'){
            is_after_decimal = true;
            current_index++;
            break;
        }

        char current_char = num_as_string[current_index];
        int current_digit = current_char - '0';

        if (current_digit < 0 || current_digit > 9) {
            break;
        }

        result *= 10;
        result += current_digit;
        current_index++;
    }

    size_t places_after_decimal = 0;
    while (num_as_string[current_index] != '\0' && is_after_decimal) {
        char current_char = num_as_string[current_index];
        int current_digit = current_char - '0';

        if (current_digit < 0 || current_digit > 9) {
            break;
        }

        result *= 10;
        result += current_digit;
        places_after_decimal++;
        current_index++;
    }

    if (places_after_decimal > 0) {
        result = result / (pow(10, places_after_decimal));
    }

    *first_nonviable_character = (char *)&num_as_string[current_index];
    return is_negative ? -result : result;
}

int main (int argc, char *argv[]) {
    char *end;
    double result = my_strtod(argv[1], &end);
    printf("Given the input %s, my function returned %lf\n", argv[1], result);
}
```

## Excercise 2

#### Write a preprocessor condition that tests whether `int` has twos' complement sign representation

```c
#include <limits.h>
#if INT_MIN < -INT_MAX
// Two’s-complement
#else
// Ones’ complement or sign-magnitude
#endif
```
