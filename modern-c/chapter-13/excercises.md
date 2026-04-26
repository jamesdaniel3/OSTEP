## Excercise 1

#### Compute the space needed to allocate a vector of doubles where there is 1 value for every living person and determine if allocating such a vector is feasible on your platform

On my system, a double is 8 bytes. There are roughly 8.3 billion living people. 

This array would require: $8 * 8.3B = 66.4B$ bytes or $66.4$ gigabytes. 

This is not feasible. 


## Excercise 2

#### Write implementations of the missing functions

They gave me all of the functions, aside for the `circular_append`, `circular_pop`, and `circular_resize`. The book says to not write the `circular_resize` function in the text, so I have skipped that one for the moment. I adjust the guard conditions to make the code easier to read and adjusted the line in `circular_element` that I commented on. Code can be found in circular_buffer_excercises.

## Excercise 3. 

#### Implement shrinking of the table; it is important to reorganize the table contents before calling realloc

Code and tests can be found in circular_buffer_excercises folder. I read this question and then spent spare time messing with it for a couple days and forgot that it mentioned using realloc. I guess it wanted me to shift and then realloc the array; I went with just freeing the array and creating a new one in it's place. I am choosing to leave my implementation in. 

## Excercise 4

#### Which value is printed by this program?

The program in question:

```c
#include <stdio.h>

unsigned i = 1;

int main(){
    unsigned i = 2;
    if (i) {
        extern unsigned i;
        printf("%u\n", i); // prints 1
    } else {
        printf("%u]n", i); // prints 2
    }
}
```

This program prints $1$. When the conditional is evaluated, the value of `i` is $2$ because the definition of `i` on the line above shadows the one that comes before `main`. Both $1$ and $2$ are truthy values, so the conditional is true. The declaration following it then shadows the one at the top of main and the output is $1$.

## Excercise 5

#### Find out which compiler arguments produce assembler output for your platform.

If I just add a `-S` flag to my arguments I am able to generate assembly; not sure if this is what the question intends for me to find as that doesn't feel platform specific. 

## Excercise 6

#### Using the fact that `p` is assigned the same value over and over again, write a C program that gets closer to what the optimized assmbler version looks like

Questions 6 and 7 are referencing this function:

```c
#include <stdio.h>

void fgoto(unsigned n) {
    unsigned j = 0;
    unsigned* p = NULL;
    unsigned* q;

AGAIN:
    if (p) {
        printf(
            "%u: p and q are %s, *p is %u\n", 
            j, 
            (q == p) ? "equal" : "unequal",
            *p
        );
    }

    q = p;
    p = &((unsigned){ j, });
    ++j;

    if (j <= n) {
        goto AGAIN;
    }
}
```

This is my best shot at what they are looking for. I find this question pretty confusing. I also may have jumped the gun with the starting j at 1 and removing the conditional; it's possible that should have been saved for the next question. 

```c
void fgoto(unsigned n) {
    unsigned j = 1;
    unsigned* p = &((unsigned){ 0, });
    unsigned* q = NULL;

AGAIN:
    if (p) {
        printf(
            "%u: p and q are %s, *p is %u\n", 
            j, 
            (q == p) ? "equal" : "unequal",
            *p
        );
    }

    q = p;
    *p = j;
    ++j;

    if (j <= n) {
        goto AGAIN;
    }
}
```

## Excercise 7

#### Even the optimized version leaves room for improvement: the inner part of the loop can still be shortened. Write a C program that explores this potential when compiled with full optmization 

```c
void fgoto(unsigned n) {
    unsigned j = 1;

AGAIN:
    printf(
        "%u: p and q are %s, *p is %u\n", 
        j, 
        (j != 1) ? "equal" : "unequal",
        j - 1
    );

    ++j;

    if (j <= n) {
        goto AGAIN;
    }
}
```

This feels wrong but I believe it would have the same output as the original function.