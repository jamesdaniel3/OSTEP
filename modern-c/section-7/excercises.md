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