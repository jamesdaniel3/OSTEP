## Excercise 1

#### Find the two reasons why this occurence of fabs in the macro expansion is not iteself expanded 

The question above is referencing this macro:

```c
#define fabs(X) \
_Generic((X), \
  float: fabsf, \
  long double: fabsl, \
  default: fabs) (X)
```

The first reason that the call to fabs is not expanded within the macro is that macros do not allow for recursion. When a macro is evaluating the text inside it does not match for its own definition (or whatever you call the first value after define). My current guess for the second reason is that we don't actually see the text fabs(X) in the macro and so even if recursion is allowed it's not able to match on anything. Macro expansion happens in the preprocessor phase, so even being able to determine the type of the variable that is used as an argument to fabs at compile time is not good enough. 

## Excercise 2

#### Extend the fabs macro to cover complex floating point types 

```c
#define fabs(X) \
_Generic((X), \
  float: fabsf, \
  long double: fabsl, \
  float _Complex: cabsf  \
  double _Complex: cabs  \
  long double _Complex:  cabsl  \
  default: fabs) (X)
```

## Excercise 3

#### Extend the `min` macro to cover all wide integer types 

This is the inital code I was given: 

```c
#define min(A, B)      \
_Generic((A) + (B),    \ 
    float: minf        \
    long double: minl, \ 
    default: mind)((A), (B))


static inline double mind(double a, double b) [[__unsequenced__]]{
    return a < b ? a : b;
}

static inline long double minl(long double a, long double b) [[__unsequenced__]] {
    return a < b ? a : b;
}

static inline float minf(float a, float b) [[__unsequenced__]] {
    return a < b ? a : b;
}
```

What counts as a wide integer type I am not certain. Regardless every addition will have the same basic shape. Here is the code with one added type:

```c
#define min(A, B)              \
_Generic((A) + (B),            \ 
    float: minf                \
    long double: minl,         \ 
    unsigned long long: minull \
    default: mind)((A), (B))


static inline double mind(double a, double b) [[__unsequenced__]]{
    return a < b ? a : b;
}

static inline long double minl(long double a, long double b) [[__unsequenced__]] {
    return a < b ? a : b;
}

static inline float minf(float a, float b) [[__unsequenced__]] {
    return a < b ? a : b;
}

static inline unsigned long long(unsigned long long a, unsigned long long b) [[__unsequenced__]] {
    return a < b  ? a : b;
}
```

## Excercise 4

#### Extend `min` to cover pointer types as well

Here it is with one pointer type added. I'm going to assume it wants me to compare the pointers and not the values they point to, because if it wanted to compare the values they point to than the user could just dereference them and use the prexisting logic 

```c
#define min(A, B)              \
_Generic((A) + (B),            \ 
    float: minf                \
    long double: minl,         \ 
    unsigned long long: minull \
    int *: minip \
    default: mind)((A), (B))


static inline double mind(double a, double b) [[__unsequenced__]]{
    return a < b ? a : b;
}

static inline long double minl(long double a, long double b) [[__unsequenced__]] {
    return a < b ? a : b;
}

static inline float minf(float a, float b) [[__unsequenced__]] {
    return a < b ? a : b;
}

static inline unsigned long long minull(unsigned long long a, unsigned long long b) [[__unsequenced__]] {
    return a < b  ? a : b;
}

static inline int * minip(int * a, int * b) [[__unsequenced__]]{
    return a < b ? a : b;
} 
```

## Excercise 5

#### Write an analogous macro for the minimum value

I was given this macro: 

```c
#define MAXVAL(X)                               \
_Generic((X),                                   \
    bool: (bool) +1,                            \
    char: (char) + CHAR_MAX,                    \
    signed char: (signed char) + SCHAR_MAX,     \
    unsigned char: (unsigned char) + UCHAR_MAX, \
    signed: INT_MAX,                            \
    unsigned: UINT_MAX,                         \
    signed long: LONG_MAX,                      \
    unsigned long: ULONG_MAX,                   \
    signed long long: LLONG_MAX,                \
    unsigned long long: ULLONG_MAX,             \
    float: FLT_MAX,                             \
    double: DBL_MAX,                            \
    long double: LDBL_MAX)
```

Here is the `MINVAL` macro:

```c
#define MINVAL(X)                               \
_Generic((X),                                   \
    bool: 0,                                    \
    char: (char) + CHAR_MIN,                    \
    signed char: (signed char) + SCHAR_MIN,     \
    unsigned char: (unsigned char) + 0,         \
    signed: INT_MIN,                            \
    unsigned: 0,                                \
    signed long: LONG_MIN,                      \
    unsigned long: 0,                           \
    signed long long: LLONG_MIN,                \
    unsigned long long: 0,                      \
    float: FLT_MIN,                             \
    double: DBL_MIN,                            \
    long double: LDBL_MIN)
```

## Excercise 6

#### By using _Generic, write a macro `PROMOTE(XT, A)` that returns the value of `A` as type `XT`, where both have a wide standard integer type. For example, PROMOTE(1u, 3) would be 3u.

```c
#define PROMOTE(XT, A)                          \
_Generic((0 + (XT) + 0),                        \
    signed: (signed) A,                         \
    unsigned: (unsigned) A,                     \
    signed long: (signed long) A,               \
    unsigned long: (unsigned long) A,           \
    signed long long: (signed long long) A,     \
    unsigned long long: (unsigned long long) A)
```

## Excercise 7

#### By using _Generic, write a macro `SIGNEDNESS(XT)` that returns false or true according to the signedness of type `XT`, where both have a wide standard integer type. For example, SIGNEDNESS(11) would be true.

```c
#define SIGNEDNESSS(XT)              \
_Generic((0 + (XT) + 0),             \
    signed: true,                    \
    unsigned: false,                 \
    signed long: true,               \
    unsigned long: false,            \
    signed long long: true,          \
    unsigned long long: false)
```

## Excercise 8

#### By using _Generic, write a macro `mix(A, B)` that computes the maximum value of `A` and `B`, where both have a wide standard integer type. If both have the same signedness, the result type should be the wider type of the two. If boht have different signedness, the return type should be an unsigend type that fits all postive values of both types. 