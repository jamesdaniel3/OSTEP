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