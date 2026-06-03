## Excercise 1

#### Rewrite the examples from subsection 10.2.2 with `inline`

Here are the functions that were provided:

```c
typedef struct rat rat;
struct rat {
    bool sign;
    size_t num;
    size_t denom;
};


int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

inline
rat rat_get(signed sign, size_t num, size_t denom) [[__unsequenced__]]{
    rat ret = {
        .sign = (sign < 0),
        .num = num,
        .denom = denom
    };

    return ret;
}

inline
rat rat_get_normal(rat x) [[__unsequenced__]]{
    size_t c = gcd(x.num, x.denom);
    x.num /= c;
    x.denom /= c;
    return x;
}

inline
rat rat_get_extended(rat x, size_t f) [[__unsequenced__]] {
    x.num *= f;
    x.denom *= f;
    return x;
}

inline
rat rat_get_prod(rat x, rat y) [[__unsequenced__]] {
    rat ret = {
        .sign = (x.sign != y.sign),
        .num = x.num * y.num,
        .denom = x.denom * y.denom
    };

    return ret;
}

inline
rat rat_get_sum(rat x, rat y) [[__unsequenced__]]{
    size_t c = gcd(x.denom, y.denom);
    size_t ax = y.denom/c;
    size_t bx = x.denom/c;

    x = rat_get_extended(x, ax);
    y = rat_get_extended(y, bx);
    assert(x.denom == y.denom);

    if (x.sign == y.sign) {
        x.num += y.num;
    } else if (x.num > y.num) {
        x.num -= y.num;
    } else {
        x.num = y.num - x.num;
        x.sign = !x.sign;
    }

    return rat_get_normal(x);
}
```

Is this it? I just added the inline attribute to the functions. It's not clear what else I should do...

## Excercise 2 

#### Revisit the function examples in chapter 7 and argue whether each should or should not be defined as inline 

```c
bool leapyear(unsigned year) {
    return !(year % 4) && ((year % 100) || !(year % 400));
}
```

I would inline this, it saves the call stack and I don't see any drawbacks.

```c
inline size_t gcd2(size_t a, size_t b) [[__unsequenced__]]{
    assert(a <= b);
    if (!a) return b;
    size_t r = b % a;
    return gcd2(r, a);
}
```

This function was written in the texbook as inline. I guess you can call it inline because it's just a suggestion but I don't think it can be inlined without 
knowing hte values of `a` and `b` at compile time, and at that point it could just be evaluated statically if the compiler was good enough or the programmer didn't use the function. 
Without knowing these values ahead of time I don't think it can be inlined because the compiler can't know how many times it would need to expand the function. So I guess you can add 
the keyword but it won't make a difference. At best, the compiler could inline the first function def and nothing past that, which I guess is an improvement.  

```c
inline size_t gcd(size_t a, size_t b) [[__unsequenced__]]{
    assert(a);
    assert(b);
    if (a < b) {
        return gcd2(a, b);
    }
    else {
        return gcd2(b, a);
    }
}
```

I think it makes sense to inline this function as it doesn't have any issues with recursion. 

```c
size_t fib(size_t n) [[__unsequenced__]] {
    if (n < 3) {
        return 1;
    }
    else {
        return fib(n - 1) + fib(n - 2);
    }
}
```

I don't think this funciton can be inlined unless you know n at compile time.