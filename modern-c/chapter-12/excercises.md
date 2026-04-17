## Excercise 1

#### Design a similar union type to investigate the bytes of a pointer type, such as `double *`

The previous union type being referenced:
```c
typedef union unsigned_inspect unsigned_inspect;
union unsigned_inspect {
    unsigned val;
    unsigned char bytes[sizeof(unsigned)];
};
```

Here is the code I am using for excercises 1-3:
```c
#include <stdio.h>
#include <stdlib.h>

typedef union double_pointer_inspect double_pointer_inspect;
union double_pointer_inspect {
    double* val;
    unsigned char bytes[sizeof(double*)];
};


int main(){
    double nums[2] = {5.5, 7.1};

    for (size_t i = 0; i < 2; i++) {
        double_pointer_inspect twofold = {.val = &nums[i]};
        printf("Value is %p\n", twofold.val);

        for (size_t j = 0; j < sizeof twofold.bytes; ++j) {
            printf("byte[%zu]: 0x%.02hhX\n", j, twofold.bytes[j]);
        }
    }
}
```

Here are some repeated terminal runs:

```
Value is 0x16b1e6ba0
byte[0]: 0xA0
byte[1]: 0x6B
byte[2]: 0x1E
byte[3]: 0x6B
byte[4]: 0x01
byte[5]: 0x00
byte[6]: 0x00
byte[7]: 0x00
Value is 0x16b1e6ba8
byte[0]: 0xA8
byte[1]: 0x6B
byte[2]: 0x1E
byte[3]: 0x6B
byte[4]: 0x01
byte[5]: 0x00
byte[6]: 0x00
byte[7]: 0x00

Value is 0x16b19aba0
byte[0]: 0xA0
byte[1]: 0xAB
byte[2]: 0x19
byte[3]: 0x6B
byte[4]: 0x01
byte[5]: 0x00
byte[6]: 0x00
byte[7]: 0x00
Value is 0x16b19aba8
byte[0]: 0xA8
byte[1]: 0xAB
byte[2]: 0x19
byte[3]: 0x6B
byte[4]: 0x01
byte[5]: 0x00
byte[6]: 0x00
byte[7]: 0x00

Value is 0x16f7f6ba0
byte[0]: 0xA0
byte[1]: 0x6B
byte[2]: 0x7F
byte[3]: 0x6F
byte[4]: 0x01
byte[5]: 0x00
byte[6]: 0x00
byte[7]: 0x00
Value is 0x16f7f6ba8
byte[0]: 0xA8
byte[1]: 0x6B
byte[2]: 0x7F
byte[3]: 0x6F
byte[4]: 0x01
byte[5]: 0x00
byte[6]: 0x00
byte[7]: 0x00
```

## Excercise 2

#### With such a union, investigate the addresses of two consecutive elements of an array

Looking at the addresses of consecutive elements of the array we can see that they are always 8 bytes apart, which makes sense because a double is 8 bytes (at least on my system).  

## Excercise 3

#### Compare the addresses of the same variable between executions

The start of our array was located at the following memory addresses in each execution: `0x16b1e6ba0`, `0x16b19aba0`, `0x16f7f6ba0`. Through many more runs I observed that the address always started with `16` and ended with `ba0`. I am not sure if that is tied to the code or something I don't understand. 

