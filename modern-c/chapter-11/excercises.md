## Excercise 1

#### Write a function that receives pointers to three objects and that shifts the values of these objects cyclically

```c
void cycle_ints(int* first_num_loc, int* second_num_loc, int* third_num_loc) {
    int temp = *third_num_loc;
    *third_num_loc = *second_num_loc;
    *second_num_loc = *first_num_loc;
    *first_num_loc = temp;
    return;
}
```

## Excercise 2

#### Implement function `rat_print` as declared in listing 10.1. This function should use -> ti access the members of its `rat*` argument. The printout should have the form `+/-nom/denum`

```c
typedef struct rat rat;
struct rat {
    bool sign;
    size_t num;
    size_t denom;
};

char const* rat_print(size_t len, char temp[len], rat const* rat_pointer) {
    if(!rat_pointer) {
        if (len > 0) {
            temp[0] = '\0';
        }
        return temp;
    }

    snprintf(temp, len, "%c%zu/%zu", (rat_pointer->sign == true ? '-' : '+'), rat_pointer->num, rat_pointer->denom);
    return temp;
}
```

## Excercise 3

#### Implement `rat_print_normalized` by combining `rat_normalize` and `rat_print`

In the following solution, I only had to write the `gcd` and `rat_print_normalized` functinos. There are better implementations of `gcd` but that' not the point. 

```c
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct rat rat;
struct rat {
    bool sign;
    size_t num;
    size_t denom;
};

char const* rat_print(size_t len, char temp[len], rat const* rat_pointer) {
    if(!rat_pointer) {
        if (len > 0) {
            temp[0] = '\0';
        }
        return temp;
    }

    snprintf(temp, len, "%c%zu/%zu", (rat_pointer->sign == true ? '-' : '+'), rat_pointer->num, rat_pointer->denom);
    return temp;
}

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

rat rat_get_normal(rat x) {
    size_t c = gcd(x.num, x.denom);
    x.num /= c;
    x.denom /= c;

    return x;
}

rat* rat_normalize(rat* rat_pointer){
    if (rat_pointer) *rat_pointer = rat_get_normal(*rat_pointer);
    return rat_pointer;
}

char const* rat_print_normalized(size_t len, char temp[len], rat* rat_pointer) {
    rat_pointer = rat_normalize(rat_pointer);
    return rat_print(len, temp, rat_pointer);
}

```

## Excercise 4

#### Implement the function `rat_dotproduct` from listing 10.1, such that it computes the dot product of arrays A and B and returns that value in `*rp`

```c
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

typedef struct rat rat;
struct rat {
    bool sign;
    size_t num;
    size_t denom;
};

size_t gcd(size_t a, size_t b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

rat rat_get_normal(rat x) {
    size_t c = gcd(x.num, x.denom);
    x.num /= c;
    x.denom /= c;

    return x;
}

rat* rat_normalize(rat* rat_pointer){
    if (rat_pointer) *rat_pointer = rat_get_normal(*rat_pointer);
    return rat_pointer;
}

rat rat_get_extended(rat x, size_t f){
    x.num *= f;
    x.denom *= f;
    return x;
}

rat* rat_extend(rat* rp, size_t f){
    if (rp) *rp = rat_get_extended(*rp, f);
    return rp;
}

rat* rat_sumup(rat* rp, rat y) {
    size_t c = gcd(rp->denom, y.denom);
    size_t ax = y.denom/c;
    size_t bx = rp->denom/c;
    rat_extend(rp, ax);
    y = rat_get_extended(y, bx);
    assert(rp->denom == y.denom);

    if (rp->sign == y.sign) {
        rp->num += y.num;
    } 
    else if (rp->num > y.num) {
        rp->num -= y.num;
    }
    else {
        rp->num = y.num - rp->num;
        rp->sign = !rp->sign;
    }

    return rat_normalize(rp);
}

rat rat_get_prod(rat x, rat y) {
    rat ret = {
        .sign = (x.sign != y.sign),
        .num = x.num * y.num,
        .denom = x.denom * y.denom,
    };

    return rat_get_normal(ret);
}

rat* rat_rma(rat* rp, rat x, rat y){
    return rat_sumup(rp, rat_get_prod(x, y));
}

rat* rat_dotproduct(rat rp[static 1], size_t n, rat const A[n], rat const B[n]){
    rp[0].sign = false;
    rp[0].num = 0;
    rp[0].denom = 1;

    for (size_t current_index = 0; current_index < n; current_index++){
        rat_rma(rp, A[current_index], B[current_index]);
    }
    
    return rp;
}
```