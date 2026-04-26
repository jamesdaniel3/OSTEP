I was given the following code:

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

int main(){
    fgoto(2);
}
```

This is the output:

```
1: p and 1 are unequal, *p is 0
2: p and 1 are equal, *p is 1
```

I don't toally understand why but I want to write down my understanding to try to lock it in. Here it is:

1. Compound literals have automatic storage. 
2. Objects with automatic storage are available for the duration of the block that they are defined in (not from the line they are defined in)

In the code above, we iterate over the compount literal definition, but we never exit the block that it is defined in. This means that it is only defined once. 

In the first iteration, `q` is set to `NULL` and p becomes a pointer to the literal of `j` (0), then we increment `j`. On the next iteration, `p` and `q` are not equal because `q` still points to `NULL`. After the print statement, `q` is set equal to `p` and `p` becomes a pointer to the literal of j (1). 

The key here is that the same memory is reused to store the literal 1 that was intiial used to store 0. Therfore, `q` also points to 1. 