## Excercise 1

#### Show that $b == a*a + 3*a + 2$

This question is referencing the following code

```c
#define square_macro(X) (X*X)

unsigned count(void){
    static counter = 0;
    ++counter;
    return counter;
}

unsigned a = count();
unsigned b = square_macro(count());
```

Using the macro replacement we can see that the last line will be evaluated as 

```c
unsigned b = (count() * count());
```

However, every time count() is called, the value it returns increases by 1. This means that, assuming `a` is equal to $1$, `b` is equal to $2 * 3$. In more general terms, 
$b = (a + 1) * (a + 2) = a^2 + a + 2a + 2 = a * a + 3 * a + 2$.

## Excercise 2

#### Show that $d == c*c + 2*c + 1$

This question is refering to the following code:

```c
// In a header file 
inline unsigned square_unsigned(unsigned x) {
    return x * x; 
}

// In a different file
unsigned count(void){
    static counter = 0;
    ++counter;
    return counter;
}

unsigned c = count();
unsigned d = square_unsigned(count());
```

Once again, we know that count increments the return value every time it is called. However, it is only evaluated once in being passed as an argument to d rather than twice when it was expanded earlier. This means that $d = (c + 1) * (c + 1) = c^2 + 2c + 1 = c * c + 2 * c + 1$.

## Excercise 3

#### Why is this so?

We are given the following macros:

```c
#define MINSIZE(X, Y) (sizeof(X) < sizeof(Y) ? sizeof(X) : sizeof(Y))
#define BYTECOPY(T, S) memcpy(&(T), &(S), MINSIZE(T, S))
```

Then the textbook says: 

> These macros fufill our requirements about function-like macros. They evaluate each argument only once, parenthesize all arguments with (), and have no hidden effects, such as unexpected control flow. 

The excercise is asking us why these macros evaluate each argument only once. 

The best answer I can scrounge up to this is that `sizeof` does not actually evaluate it's operands; it runs at compile time because it just needs to know the type of the operand to return it. Taking this to be true, the `MINSIZE` macro would never evaluate it's operands and the `BYTECOPY` macro would only evaluate its operands once each. 

The main issue with this is that from what I can tell, is that VLA's are an exception to the to this no evaluation rule. Because of this, if you were to have a line like 

```c
int first_vla[dynamic_size];
int second_vla[dynamic_size];
MINSIZE(first_vla[function_with_side_effect()], second_vla[function_with_side_effect()]);
```

This in theory would have the same problem as example 1 because the function would have to be executed multiple times. 

## Excercise 4

#### Variadic functions that only receive arguments that are all the same type can be replaced by a variadic macro and an inline function that takes an array. Do it. 

If I originally had: 
```c
int my_variadic_func(size_t num_args, ...) {
    int result = 0;
    va_list va;
    va_start(va);
    for (size_t i = 0; i < num_args; i++) {
        result += va_arg(va, int);
    }
    va_end(va);
    return result;
}
```

I could replace it with the following macros:

```c
#include <stdlib.h>

#define ALEN(...) ALEN0(__VA_ARGS__, \
0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08, \
0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00)

#define ALEN0(_00, _01, _02, _03, _04, _05, _06, _07, \
_08, _09, _0A, _0B, _0C, _0D, _0E, _0F, ...) _0F

#define my_variadic_func(...)  \
my_variadic_func0(ALEN(__VA_ARGS__), ((int const [ALEN(__VA_ARGS__)]){__VA_ARGS__}))

inline my_variadic_func0(size_t num_args, int const arr[static num_args]) {
    int result = 0;
    for (int index = 0; index < num_args; index++) {
        result += arr[index];
    }

    return arr;
}
```

## Excercise 5

#### Just as we did for `DEFAULT2`, compute the results for all possible combinations of arguments for DEFAULT 3

We are given the following macros that can provide default values for empty arguments:

```c
#define DEFAULT1(DEF0, ...) \
ID_I ## __VA_OPT__(Iplus_DEFAULT) \
(DEF0 __VA_OPT__(,) __VA_ARGS__) 

#define ID_IIplus_DEFAULT(_01, ...) ID_I(__VA_ARGS__)

#define ID_()
#define ID_I(_01)
#define ID_II(_01, _02)
#define ID_III(_01, _02, _03)

#define DEFAULT2(DEF0, DEF1, ...) \
ID_II ## __VA_OPT__ (Iplus_DEFAULT) \
(DEF0, DEF1 __VA_OPT__(,) __VA_ARGS__)

#define ID_IIIplus_DEFAULT(DEF0, DEF1, _01, ...) \
    DEFAULT1(DEF0, _01), DEFAULT1(DEF1, __VA_ARGS__)

#define DEFAULT3(DEF0, DEF1, DEF2, ...) \
ID_III ## __VA_OPT__ (Iplus_DEFAULT) \
(DEF0, DEF1, DEF2 __VA_OPT__(,) __VA_ARGS__)

#define ID_IIIIplus_DEFAULT(DEF0, DEF1, DEF2, _01, ...) \
DEFAULT1(DEF0, _01) DEFAULT2(DEF1, DEF2, __VA_ARGS__)
```

After showing us `DEFAULT2`, we are given the following breakdown:

```
DEFAULT2(a, b)  -> ID_II(a, b) -> a, b
DEFAULT2(a, b,)  -> ID_II(a, b) -> a, b
DEFAULT2(a, b, A)  -> ID_IIIplus_DEFAULT(a, b, A) -> DEFAULT1(a, A), DEFAULT1(b, ) -> A, b
DEFAULT2(a, b, A,)  -> ID_IIIplus_DEFAULT(a, b, A) -> DEFAULT1(a, A), DEFAULT1(b, ) -> A, b
DEFAULT2(a, b, A, B)  -> ID_IIIplus_DEFAULT(a, b, A, B) -> DEFAULT1(a, A), DEFAULT1(b, B) -> A, B
DEFAULT2(a, b, , B)  -> ID_IIIplus_DEFAULT(a, b, , B) -> DEFAULT1(a, ), DEFAULT1(b, B) -> a, B
DEFAULT2(a, b, , , C)  -> ID_IIIplus_DEFAULT(a, b, , , C) -> DEFAULT1(a, ), DEFAULT1(b, , C) -> A, error
```

Here are all the possible args that can be sent to `DEFAULT3` and the results:

```
DEFAULT3(a, b, c) -> ID_III(a, b, c) -> a, b, c
DEFAULT3(a, b, c,) -> ID_III(a, b, c) -> a, b, c
DEFAULT3(a, b, c, A) -> ID_IIIIplus_DEFAULT(a, b, c, A) -> DEFAULT1(a, A), DEFAULT2(b, c) -> A, b, c
DEFAULT3(a, b, c, A,) -> ID_IIIIplus_DEFAULT(a, b, c, A) -> DEFAULT1(a, A), DEFAULT2(b, c) -> A, b, c
DEFAULT3(a, b, c, A, B) -> ID_IIIIplus_DEFAULT(a, b, c, A, B) -> DEFAULT1(a, A), DEFAULT2(b, c, B) -> DEFAULT1(a, A), DEFAULT1(b, B),  DEFAULT1(c, ) -> A, B, c
DEFAULT3(a, b, c, A, B,) -> ID_IIIIplus_DEFAULT(a, b, c, A, B) -> DEFAULT1(a, A), DEFAULT2(b, c, B) -> DEFAULT1(a, A), DEFAULT1(b, B),  DEFAULT1(c, ) -> A, B, c
DEFAULT3(a, b, c, A, B, C) -> ID_IIIIplus_DEFAULT(a, b, c, A, B, C) -> DEFAULT1(a, A), DEFAULT2(b, c, B, C) -> DEFAULT1(a, A), DEFAULT1(b, B),  DEFAULT1(c, C) -> A, B, C
DEFAULT3(a, b, c, , B, C) -> ID_IIIIplus_DEFAULT(a, b, c, , B, C) -> DEFAULT1(a, ), DEFAULT2(b, c, B, C) -> DEFAULT1(a, ), DEFAULT1(b, B),  DEFAULT1(c, C) -> a, B, C
DEFAULT3(a, b, c, , , C) -> ID_IIIIplus_DEFAULT(a, b, c, , , C) -> DEFAULT1(a, ), DEFAULT2(b, c, , C) -> DEFAULT1(a, ), DEFAULT1(b, ),  DEFAULT1(c, C) -> a, b, C
DEFAULT3(a, b, c, , B, ) -> ID_IIIIplus_DEFAULT(a, b, c, , B, ) -> DEFAULT1(a, ), DEFAULT2(b, c, B, ) -> DEFAULT1(a, ), DEFAULT1(b, B),  DEFAULT1(c, ) -> a, B, c
DEFAULT3(a, b, c, A, , C) -> ID_IIIIplus_DEFAULT(a, b, c, , , C) -> DEFAULT1(a, ), DEFAULT2(b, c, , C) -> DEFAULT1(a, ), DEFAULT1(b, ),  DEFAULT1(c, C) -> a, b, C
DEFAULT3(a, b, c, A, , , D) -> ID_IIIIplus_DEFAULT(a, b, c, , , , D) -> DEFAULT1(a, ), DEFAULT2(b, c, , , D) -> DEFAULT1(a, ), DEFAULT1(b, ),  DEFAULT1(c, , D) -> error
```