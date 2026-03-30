## Excercise 1

#### Consider a macro `sum(a, b)` that is implemented as a+b. What is the result of `sum(5, 2)*7`?

Take the following c code 
```c
#define sum(a, b) a + b

int main(){
    int foo = sum(5, 2)*7;
    return;
}
```

The line in question would be converted to 

```c
int foo = 5 + 2*7;
```

This would be equal to $19$, when you would probably anticipate that the expression would be equal to $49$ based on the code before macro expansion. 

## Excercise 2

#### Let `max(a, b)` be implemented as `((a) < (b) ? (b) : (a))`. What happens for `max(i++, 5)`?

`max(i++, 5)` would become 

```c
((i++) < (5) ? (5) : (i++))
```

This is problematic because i++ is an assignment operator. Imagine we had `i = 5`. We would expect that `max(i++, 5)` would result in `i`, which would be equal to 6 after the `++` operator ran. However, the result would actually be 7, because the operator was used both in the comparison step and the result. 

## Excercise 3

#### The function `rat_get_prod` can produce intermediate values that may cause it to produce wrong results, even if the mathematical result of the multiplication is representable in `rat`. How is that possible?

Here is the function in question:

```c
#include <stdbool.h>
#include <stddef.h>

typedef struct rat rat;
struct rat {
    bool sign;
    size_t num;
    size_t denom;
};

rat rat_get_normal(rat x) [[__unsequenced__]] {
    size_t c = gcd(x.num, x.denom);
    x.num /= c;
    x.denom /= c;

    return x;
}

rat rat_get_prod(rat x, rat y) [[__unsequenced__]] {
    rat ret = {
        .sign = (x.sign != y.sign),
        .num = x.num * y.num,
        .denom = x.denom * y.denom,
    };

    return rat_get_normal(ret);
}
```

The issue is that we only normalize the numbers after we have already done the multiplication. If we had large enough numbers to create overflow on either of these lines:

```c
    .num = x.num * y.num,
    .denom = x.denom * y.denom,
```

Then the result would wrap around and we would get a wrong answer; despite the fact that these values could have been reduced. 

If the max value of a size_t type value is $18,446,744,073,709,551,615$

We could have the following numbers: 

```
rat first_num = {
    .sign = false,
    .num = 5,000,000,000,000,000,000,
    .denom = 3,000,000,000,000,000,000,
};

rat second_num = {
    .sign = false,
    .num = 8,
    .denom = 3,
};
```

Because we don't normalize the values before calculating, we will see the following: 

$5,000,000,000,000,000,000 * 8 = 40,000,000,000,000,000,000$

which is too big to fit in `size_t`, so it get's modulo'd at becomes:

$3,106,511,852,600,000,000$.

The result is $3,106,511,852,600,000,000 / 9,000,000,000,000,000,000$ instead of $40/9$. 


## Excercise 4

#### Reimplement the `rat_get_prod` function so that it produces a correct result every time the mathematical result is representable in a `rat`. This can be done with two calls to `rat_get_normal` instead of one. 

```c
rat rat_get_prod(rat x, rat y) [[__unsequenced__]] {
    rat normalized_x = rat_get_normal(x);
    rat normalized_y = rat_get_normal(y);

    rat ret = {
        .sign = (x.sign != y.sign),
        .num = normalized_x.num * normalized_y.num,
        .denom = normalized_x.denom * normalized_y.denom,
    };

    return ret;
}
```