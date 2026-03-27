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
## Excercise 8

#### The second return of `hexatridecimal` makes an assumption about the relation between `a` and 'A'. What is it?

Excercises 8, 9, and 10 refer to this function:

```c
#include <assert.h>
/* supposes that lowercase characters are contiguous */
static_assert('z' - 'a' == 25, "alphabetic characters not contiguous");
#include <ctype.h>

unsigned hexatridecimal(int a){
    /* Converts an alphanumeric digit to an unsigned
    '0' ... '9' => 0 ... 9u
    'A' ... 'Z' => 10 ... 35u
    'a' ... 'z' => 10 ... 35u
    */
    if (isdigit(a)) {
        return a - '0';
    }
    else {
        a = toupper(a);
        return (isupper(a)) ? 10 + (a - 'A') : -1;
    }
}
```

This code makes the assumption that the upper case letters are stored contiguously and therefore that `a` must be >= 'A' and `a` - 'A' <= 25 if `a` is an uppercase letter. 

## Excercise 9

#### Describe an error scenario in which this assumption is not fufilled

If the encodings for upper case letters were for any reason spaced out or stored in some way other than 'A', 'B', ... , 'Z', than sending in an uppercase letter could result in a number outside of the range from [10, 35]. 

## Excercise 10

#### Fix this bug: that is, rewrite this code such that it makes no assumption about the relation between `a` and 'A'

```c
#include <assert.h>
/* supposes that lowercase characters are contiguous */
static_assert('z' - 'a' == 25, "alphabetic characters not contiguous");
#include <ctype.h>

unsigned hexatridecimal(int a){
    /* Converts an alphanumeric digit to an unsigned
    '0' ... '9' => 0 ... 9u
    'A' ... 'Z' => 10 ... 35u
    'a' ... 'z' => 10 ... 35u
    */
    if (isdigit(a)) {
        return a - '0';
    }
    else {
        a = tolower(a);
        return (islower(a)) ? 10 + (a - 'a') : -1;
    }
}
```

We can use the lowercase letters which were checked with the inital staic_assert. 

## Excercise 11

#### Implement a function `find_prefix` as needed by Strtoul

```c
#include <limits.h>
#include <errno.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

size_t find_prefix(const char* original_string, size_t starting_position, const char* prefix){
    size_t current_index = 0;

    while (prefix[current_index] != '\0' && 
        original_string[starting_position + current_index] != '\0' &&
        tolower(original_string[starting_position + current_index]) == tolower(prefix[current_index])) {
        current_index++;
    }

    return current_index;
}

unsigned long Strtoul_inner(const char* original_string, unsigned base){
    // This is basically just a stub because the textbook didn't define this function or ask me to 
    return strtoul(original_string, NULL, base);
}

unsigned long Strtoul(char const s[static 1], unsigned base) {
    if (base > 36u) {
        errno = EINVAL;
        return ULONG_MAX;
    }

    size_t i = strspn(s, " \f\n\r\t\v");
    bool switch_sign = false;

    switch (s[i]) {
        case '-':
            switch_sign = true;
            [[fallthrough]];
        case '+':
            ++i; 
    }

    if (!s[i]) return 0;

    if (!base || base == 16 || base == 2) { // look for prefix
        size_t adj = find_prefix(s, i, "0x");

        switch (adj){
            case 2: // There is 0x or 0X prefix
                if (!base || base == 16) base = 16;
                // if we were looking for another base, than the x ends our processing
                else return 0;
                break;
            case 1: // There is a 0 prefix
                adj = find_prefix(s, i, "0b");
                switch (adj){
                    case 1: // There is only a 0 prefix
                        if (!base) base = 8;
                        break;
                    default: // There is an 0b or 0B prefix
                        if (!base || base == 2) base = 2;
                        // if we are looking for another base, than the b ends our processing
                        else return 0;
                        break;
                }
                break;
            default:
                if (!base) base = 10;
                break;
        }
        i += adj;
    }
    if (!s[i]) return 0; // maybe the prefix 0 was the only digit 

    unsigned long ret = Strtoul_inner(s, base); // ignored second argument given in the textbook to make the stub make more sense 
    return (switch_sign) ? -ret : ret;
}

int main() {
    /* Some random test cases */
    printf("Strtoul test calls:\n");
    printf("0xFFFF: %lu\n", Strtoul("0xFFFF", 0));
    printf("-0xFFFF: %lu\n", Strtoul("-0xFFFF", 16));
    printf("+0xFFFF: %lu\n", Strtoul("+0xFFFF", 32));
    printf("-0123: %lu\n", Strtoul("-0123", 0));
    printf("0: %lu\n", Strtoul("-0", 0));
}
```