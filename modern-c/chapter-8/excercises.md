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

## Excercise 3

#### Observe the behavior of the program by running it with zero, one, and two command-line arguments

Here is the program the question references:

```c
#include <stdio.h>

/* delay execution with some crude code, should use thrd_sleep, once we have that */
void delay(double secs){
    double const magic = 4E8;
    unsigned long long const nano = secs * magic;
    for (unsigned long volatile count = 0; count < nano; ++count) {
        /* nothing here */
    }
}

int main(int argc, [[maybe_unused]] char* argv[argc + 1]) {
    fputs("waiting 10 seconds for you to stop me", stdout);
    if (argc < 3) fflush(stdout);
    for (unsigned i = 0; i < 10; ++i){
        fputc('.', stdout);
        if (argc < 2) fflush(stdout);
        delay(1.00);
    }
    fputs("\n", stdout);
    fputs("You did ignore me, so bye bye\n", stdout);
}
```

Here is the terminal output:

```
(base) jamesdaniel@MacBook-Pro-6 chapter-8 % ./temp
waiting 10 seconds for you to stop me..........
You did ignore me, so bye bye
(base) jamesdaniel@MacBook-Pro-6 chapter-8 % ./temp firstArg
waiting 10 seconds for you to stop me..........
You did ignore me, so bye bye
(base) jamesdaniel@MacBook-Pro-6 chapter-8 % ./temp firstArg secondArg
waiting 10 seconds for you to stop me..........
You did ignore me, so bye bye
```

It all looks the exact same, but it came out with different timing. In the first case, we saw the first line, then one dot at a time, then the second line. In the second case, we saw the first line, then all the dots and the second line printed at once. In the third case, we saw everything at once. All programs seemed to execute in the same amount of time.

## Excercise 4

#### Under what circumstances will this program finish with success or failure return codes?

Excercises 4-8 reference the following program:

```c
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

enum { buf_max = 32 };

int main(int argc, char* argv[argc + 1]){
    int ret = EXIT_FAILURE;
    char buffer[buf_max] = {};
    for (int i = 1; i < argc; ++i) {
        FILE* instream = fopen(argv[i], "r");
        if (instream) {
            while (fgets(buffer, buf_max, instream)) {
                fputs(buffer, stdout);
            }
            fclose(instream);
            ret = EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Could not open %s: ", argv[i]);
            perror(0);
            errno = 0;
        }
    }
    return ret;
}
```

The program returns EXIT_SUCCESS if at least one file is successfully opened and processed. It only returns EXIT_FAILURE if all fopen calls fail.

## Excercise 5

#### Surprisingly, this program even works for files with lines that have more than 31 characters. Why?

We set fgets to read at most, `buf_max` characters from the stream. This means it reads until if fills up the buffer (31 chars), finds the end, or fails, and then it returns and the loop condition is evaluated. Inside the loop we write out the current content of the buffer. fgets then runs again and overwrites the buffer content.

## Excercise 6

#### Have the program read from `stdin` if no command-line argument is given.

```c
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

enum { buf_max = 32 };

int main(int argc, char* argv[argc + 1]){
    int ret = EXIT_FAILURE;
    char buffer[buf_max] = {};
    for (int i = 1; i < argc; ++i) {
        FILE* instream = fopen(argv[i], "r");
        if (instream) {
            while (fgets(buffer, buf_max, instream)) {
                fputs(buffer, stdout);
            }
            fclose(instream);
            ret = EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Could not open %s: ", argv[i]);
            perror(0);
            errno = 0;
        }
    }

    if (argc == 1) {
        while (fgets(buffer, buf_max, stdin)) {
            fputs(buffer, stdout);
        }
        ret = EXIT_SUCCESS;
    }

    return ret;
}
```

## Excercise 7

#### Have the program precede all output lines with line numbers if the first command-line argument is "-n"

```c
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

enum { buf_max = 32 };

int main(int argc, char* argv[argc + 1]){
    int ret = EXIT_FAILURE;
    char buffer[buf_max] = {};
    bool show_line_nums = false;
    bool is_new_line = true;
    size_t current_line_num = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        show_line_nums = true;
    }

    if (argc == 1 || (argc == 2 && show_line_nums == true)) {
        while (fgets(buffer, buf_max, stdin)) {
            if (show_line_nums && is_new_line) {
                printf("%zu: ", current_line_num);
                current_line_num++;
                is_new_line = false;
            }
            fputs(buffer, stdout);
            if (strchr(buffer, '\n') != NULL) {
                is_new_line = true;
            }
        }
        ret = EXIT_SUCCESS;
    }

    for (int i = 1; i < argc; ++i) {
        if (i == 1 ) {
            show_line_nums = true;
        }

        FILE* instream = fopen(argv[i], "r");
        if (instream) {
            if (show_line_nums) {
                printf("%zu: ", current_line_num);
                current_line_num++;
            }
            while (fgets(buffer, buf_max, instream)) {
                fputs(buffer, stdout);
                if (show_line_nums && strchr(buffer, '\n') != NULL) {
                    printf("%zu: ", current_line_num);
                    current_line_num++;
                }
            }
            fclose(instream);
            ret = EXIT_SUCCESS;
        } else {
            fprintf(stderr, "Could not open %s: ", argv[i]);
            perror(0);
            errno = 0;
        }
    }

    return ret;
}
```
