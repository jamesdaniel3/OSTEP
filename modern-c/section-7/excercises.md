## Excercise 1

#### Show that a call `fib(n)` induces $F_n$ leaf calls

We know that the first two fibonacci numbers $F_1$ and $F_2$ are both equal to 1.

The function to calculate the value of the nth number in the fibonacci sequence is:

```c
size_t fib(size_t n) {
    if (n < 3) {
        return 1;
    }
    else {
        return fib(n - 1) + fib(n - 2);
    }
}
```

Because $n < 3$ is a base case, `fib(1)` and `fib(2)` both have 1 leaf call (the only call), allowing the condition to hold for them. From here, the above statement can be understood by understanding the recursive nature of the function. Here is an example of what I mean.

$F_3 = 2$ We know that `fib(3)` is calculated by summing `fib(1)` and `fib(2)`, and we know that each of those has 1 leaf call. Therefore, `fib(3)` results in 2 leaf calls, and the rule remains true. $F_4 = 3$ and is calculated by taking the sum of `fib(3)` and `fib(2)`, which we know result in 2 and 1 leaf calls respectively.

By induction, assume that `fib(k)` produces $F_k$ leaf calls and `fib(k - 1)` produces $F_{k-1}$ leaf calls. Consider `fib(k + 1)`. It will make two recusive calls: `fib(k)` and `fib(k + 1)`; therefore, the total number of leaf calls will be equal to the leaf calls of these two functions $F_k + F_{k - 1}$. We know that by definition the fibonacci sequence also states that $F_{k + 1} = F_k + F_{k - 1}$. So, `fib(k+1)` produces $F_k+1$ leaf calls.

## Excercise 2

#### Measure the times for calls to `fib(n)` with `n` set to different values. On POSIX systems, you can use `/bin/time` to measure the run time of a program's execution.

Code

```c
#include <stddef.h>
#include <stdlib.h>

size_t fib(size_t n) [[__unsequenced__]] {
    if (n < 3) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
}

int main (int argc, char* argv[]){
    char *end = NULL;
    int n = strtol(argv[1], &end, 10);
    fib(n);
}
```

Terminal outputs

```
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 30
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 35
        0.04 real         0.03 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 40
        0.38 real         0.37 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 45
        4.17 real         4.14 user         0.01 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 46
        6.79 real         6.69 user         0.02 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 47
       10.86 real        10.80 user         0.02 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 48
       17.46 real        17.40 user         0.02 sys
```

## Excercise 3

#### (Reworded) Prove that a memoized implementation of the fibinacci number calculator has a linear time complexity

From an intuition perspective this one isn't to bad, especially if you think about the tree. First, here is the function we were given:

```c
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t fibCacheRec(size_t n, size_t cache[static n]) {
    if (!cache[n - 1]) {
        cache[n - 1] = fibCacheRec(n - 1, cache) + fibCacheRec(n - 2, cache);
    }
    return cache[n - 1];
}

size_t fibCache(size_t n) {
    if (n < 3) {
        return 1;
    }
    #if __STDC_VERSION__ > 202311L
        size_t cache[n] = {};
    #else
        size_t cache[n]; memset(cache, 0, n*sizeof(*cache));
    #endif
    cache[0] = 1; cache[1] = 1;
    return fibCacheRec(n, cache);
}

int main (int argc, char* argv[]){
    char *end = NULL;
    int n = strtol(argv[1], &end, 10);
    fibCache(n);
}
```

In the previous implementation of `fib(n)`, the function has the process all the way down to the base case ~ $2^n$ times. However, in this new function, process all the way down to a base case once (always taking the path for $- 1$ first) and then we have all of the results stored so none of the recursive calls require any processing. I have included pictures below for an example; I think this is more clear with even larger trees but they are a bit tedius to make:

Here is the function calls $without$ memoization:

![Graph of recursive calls in fib function without memoization](/modern-c/section-7/fib_no_memo.png)

Here is the function calls $with$ memoization:

![Graph of recursive calls in fib function with memoization](/modern-c/section-7/fib_with_memo.png)

The key thing to understand here is that the calls move all way through the leftmost branch of the tree first. So the first node to return a value is the bottom left. This will allow you to cut off the recursion of any nodes on the right because their value will have always already been found. The effect is much more pronounced as the value of $n$ gets bigger.

## Exercise 4

#### Measure times for `fibCache(n)` calls with the same values as for fib

There's really no sense in measuring with the same values, they will all be essentially instant. I will take the values much higher to show the true performance impact. The code can be seen above; here is the terminal output:

```
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 30
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 50
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 100
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 1000
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 10000
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 100000
        0.00 real         0.00 user         0.00 sys
```

Side note: this program seems to blow up before any real slowness can be seen

## Excercise 5

#### Use an iteration statement to transforn `fib2rec` into a nonrecursive function `fib2iter`

`fib2rec` was given and is the following:

```c
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void fib2rec(size_t n, size_t buf[static 2]) [[__unsequenced__]] {
    if (n > 2) {
        size_t res = buf[0] + buf[1];
        buf[1] = buf[0];
        buf[0] = res;
        fib2rec(n - 1, buf);
    }
}

size_t fib2(size_t n){
    size_t res[2] = {1, 1};
    fib2rec(n, res);
    return res[0];
}

int main (int argc, char* argv[]){
    char *end = NULL;
    int n = strtol(argv[1], &end, 10);
    fib2(n);
}
```

An iterative version of this function would look like this (I also added printing b/c I wanted to see values):

```c
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void fib2iter(size_t n, size_t buf[static 2]) [[__unsequenced__]] {
    while (n > 2) {
        size_t res = buf[0] + buf[1];
        buf[1] = buf[0];
        buf[0] = res;
        n--;
    }
}

size_t fib2(size_t n){
    size_t res[2] = {1, 1};
    fib2iter(n, res);
    return res[0];
}

int main (int argc, char* argv[]){
    char *end = NULL;
    int n = strtol(argv[1], &end, 10);
    int result = fib2(n);
    printf("Result: %d\n", result);
}
```

## Execercise 6

#### Measure times for `fib2(n)` calls with the same values as fib

I'm going to use the iterative implementation shown above with the printing removed. Here are the console outputs:

```
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 100
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 120
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 500
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 1000
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 10000
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 100000
        0.00 real         0.00 user         0.00 sys
(base) jamesdaniel@MacBook-Pro-6 section-7 % /usr/bin/time ./temp 200000
        0.00 real         0.00 user         0.00 sys
```

This appears to be the best implementation yet. It doesn't have to worry about memory on the stack frame or with a huge array being allocated in the stack and it still works in `O(n)` time.
