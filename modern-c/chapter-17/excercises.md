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

