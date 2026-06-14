/*
Macros that test an expression for a certain feature without evaluating it can be helpful to distinguish taken branches of code 
at compilation time. Therefore, in following, the macros you write should never evaluate their argument and, in general, 
by themselves, result in an integer constant expression of type bool.

- Write is_null_pointer_constant to test an expression X (that is either a zero integer constant or a void * pointer)
  if it is a null pointer constant. Use the fact that an expression of the form: true ? (struct toto*)0 : (X) 
  has type struct toto* if X is a null pointer constant but type void* if X is a pointer of type void* that is not a 
  null pointer constant. 

- Write is_zero_ice that detects whether the argument is an integer constant expression of value zero (ICEv0). 
  Use the fact that such an ICEV0 is a null pointer constant if it is cast to void* and that any other integer expression, 
  even if cast to void *, is never a null pointer constant. 

- Write isice that detcts whether the argument is an integer constant expression. 
*/


/*
                    MY NOTES
For starters, you have to keep in mind that the expression true ? (struct toto*)0 : (X) itself has a type. It is not either of the types
that it evaluates to at run time, it is instead parsed an assigned it's own type. 

I can't figure out how to do part 3. There are some writeups like the ones below, but they evaluate the expression:
https://stackoverflow.com/questions/49481217/linux-kernels-is-constexpr-macro
*/

#include <stdbool.h>
#include <stdio.h>

#define IS_INT_POINTER(x) \
  _Generic((x), \
    int *: true, \
    default: false)

#define is_null_pointer_constant(X) IS_INT_POINTER(1 ? (int *)0 : (X))

#define is_zero_ice(X) is_null_pointer_constant((void *)(X))

#define is_ice(x) IS_INT_POINTER((1 ? (void *)((x)*0) : (int *) 0))

void * p = NULL;

int main() {
  bool first_true = is_null_pointer_constant(0);
  bool first_false = is_null_pointer_constant(p);

  bool second_true = is_zero_ice(0);
  bool second_false = is_zero_ice(9);

  int temp = 0;
  bool third_true = is_zero_ice(0);
  bool third_false = is_zero_ice(temp);

  printf("%d\n", first_true);
  printf("%d\n", first_false);
  printf("--------------\n");
  printf("%d\n", second_true);
  printf("%d\n", second_false);
  printf("--------------\n");
  printf("%d\n", third_true);
  printf("%d\n", third_false);
}