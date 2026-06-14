/*
    Challenge 19

The typeof operator and _Generic in combination are quite powerful and can be used to provide type traits 
- that is, operations that can be used at compile time to query certain properties of types. This then 
allows us to implement type-generic features (such as the maximum or minimum of two values) more efficeintly 
if the types of argument values fulfill certain conditions. In the following, the type-trait macros that you 
write should be able to accept as the argument XT either an expression (without exposed comma) or a type
name and result in values of type bool that are themselves integer constant expressions. 
*/


/*
As a warmup, write a macro tozero that uses typeof to provide a zero of the type of a given arithmetic 
expression or type name and a similar one tonull that provides a null pointer for pointer expressions or types. 
The result should never evaluate the argument XT and be an integer, arithmetic, or adddress constant expression, 
respectively, regardless of the circumstances. 
*/
#define tozero(XT) ((typeof(XT))0)
#define tonull(XT) ((typeof(XT))0) // 0 is a null pointer constant when evaluated as a pointer

/*
C23 has many integer types (at least 139), so a complete listing of these types in generic selections is not 
feasible. Use tozero and is_zero_ice from challenge 18 to test whether XT has an integer type isinteger.
*/
#define IS_INT_POINTER(x) \
  _Generic((x), \
    int *: true, \
    default: false)

#define is_null_pointer_constant(X) IS_INT_POINTER(1 ? (int *)0 : (X))
#define is_zero_ice(X) is_null_pointer_constant((void *)(X))
#define isinteger(XT) is_zero_ice(tozero(XT))

/*
Use isice from challenge 18 to detect whether XT is a VLA, isvla. Use the fact that sizeof for a VLA is never 
an integer constant expression.
*/
#define is_ice(x) IS_INT_POINTER((1 ? (void *)((x)*0) : (int *) 0)) // THIS DOESN'T WORK BUT I HAVEN'T FOUND A WORKING VERSION, PRETEND IT WORKS
#define isvla(XT) is_ice(sizeof(XT))

/*
For pointer types, write macros that test whether the target type is const- or volatile-qualified 
(is_const_target and is_volatile_target, respectively). Use these macros to test whether the argument itself is 
qualified, is_const and is_volatile.
*/
#define is_const_taget(XT) \
    _Generic(*(XT), \
        const typeof(*(XT)): true, \
        default: false)

#define is_volatile_target(XT) \
    _Generic(*(XT), \
        volatile typeof(*(XT)): true, \
        default: false)

#define is_const(XT) is_const_taget(&XT)
#define is_volatile(XT) is_volatile_target(&XT)

/*
Write a macro is_potentially_negative that checks whether the type may admit negative values. Test your macro on 
signed and unsigned integer types, char, and real floating point types.
*/
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

#define SIGNEDNESS(XT)              \
_Generic((0 + (XT) + 0),             \
    signed: true,                    \
    unsigned: false,                 \
    signed long: true,               \
    unsigned long: false,            \
    signed long long: true,          \
    unsigned long long: false)

#define is_potentiall_negative(XT) (SIGNEDNESS(MINVAL(XT)))

/*
Write macros issigned and isunsigned that check whether the type is signed or unsigned, respetively. Floating 
point types, pointers, or char should never be identified as signed or unsigned.
*/

#define is_signed(XT)                \
_Generic((0 + (XT) + 0),             \
    signed: true,                    \
    signed long: true,               \
    signed long long: true,          \
    default: false)

#define is_unsigned(XT)              \
_Generic((0 + (XT) + 0),             \
    unsigned: false,                 \
    unsigned long: false,            \
    unsigned long long: false        \
    default: false)

/*
Write test macros is_void_target for void pointers and is_void for void expressions. 
*/

#define is_void_target(XT)  \
_Generic(XT,                \
    void *: true,           \
    default: false)

#define is_void(XT)  \
_Generic(XT,                \
    void: true,           \
    default: false)