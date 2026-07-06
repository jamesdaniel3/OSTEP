### First, write a simple program called `null.c` that creates a pointer to an integer, sets it to `NULL`, and then tries to dereference it. Compile this into an executable called `null`. What happens when you run this program?

I get a seg fault

### Next, compile this program with symbol information included (with the `-g` flag). Doing so let’s put more information into the executable, enabling the debugger to access more useful information about variable names and the like. Run the program under the debugger by typing `gdb ./null` and then, once `gdb` is running, typing `run`. What does `gdb` show you?

```
Program received signal SIGSEGV, Segmentation fault.
0x0000aaaaaaaa0774 in main () at null.c:6
6		return *a;
```

### Finally, use the `valgrind` tool on this program. We’ll use `memcheck` that is a part of `valgrind` to analyze what happens. Run this by typing in the following: `valgrind --leak-check=yes ./null`. What happens when you run this? Can you interpret the output from the tool?

```
james@james-linux-vm:~$ valgrind --leak-check=yes ./null
==1712== Memcheck, a memory error detector
==1712== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==1712== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==1712== Command: ./null
==1712==
==1712== Invalid read of size 4
==1712==    at 0x4000774: main (null.c:6)
==1712==  Address 0x0 is not stack'd, malloc'd or (recently) free'd
==1712==
==1712==
==1712== Process terminating with default action of signal 11 (SIGSEGV)
==1712==  Access not within mapped region at address 0x0
==1712==    at 0x4000774: main (null.c:6)
==1712==  If you believe this happened as a result of a stack
==1712==  overflow in your program's main thread (unlikely but
==1712==  possible), you can try to increase the size of the
==1712==  main thread stack using the --main-stacksize= flag.
==1712==  The main thread stack size used in this run was 8388608.
==1712==
==1712== HEAP SUMMARY:
==1712==     in use at exit: 0 bytes in 0 blocks
==1712==   total heap usage: 0 allocs, 0 frees, 0 bytes allocated
==1712==
==1712== All heap blocks were freed -- no leaks are possible
==1712==
==1712== For lists of detected and suppressed errors, rerun with: -s
==1712== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
Segmentation fault         (core dumped) valgrind --leak-check=yes ./null
```

The above is the output `valgrind` gave. We see the seg fault at the bottom and we see that the memory access happens on line 6 of our program. 

### Write a simple program that allocates memory using `malloc()` but forgets to free it before exiting. What happens when this program runs? Can you use `gdb` to find any problems with it? How about `valgrind` (again with the `--leak-check=yes` flag)?

normal runs and `gdb` do not find issues but `valgrind` does

```
==1837== HEAP SUMMARY:
==1837==     in use at exit: 40 bytes in 1 blocks
==1837==   total heap usage: 2 allocs, 1 frees, 1,064 bytes allocated
==1837==
==1837== 40 bytes in 1 blocks are definitely lost in loss record 1 of 1
==1837==    at 0x48A5618: malloc (vg_replace_malloc.c:447)
==1837==    by 0x40007F7: main (temp.c:5)
==1837==
==1837== LEAK SUMMARY:
==1837==    definitely lost: 40 bytes in 1 blocks
==1837==    indirectly lost: 0 bytes in 0 blocks
==1837==      possibly lost: 0 bytes in 0 blocks
==1837==    still reachable: 0 bytes in 0 blocks
==1837==         suppressed: 0 bytes in 0 blocks
```

### Write a program that creates an array of integers called `data` of size 100 using `malloc`; then, set `data[100]` to zero. What happens when you run this program? What happens when you run this program using `valgrind`? Is the program correct?

THe program is not correct. GCC is cool and gives a stack smashing warning when I run it. I don't feel like valgrind really provided any new information 

### Create a program that allocates an array of integers (as above), frees them, and then tries to print the value of one of the elements of the array. Does the program run? What happens when you use `valgrind` on it?

This was caught in compilation but it was a warning rather than an error. The program actually is running but does not print the same value every time. Valgrind catches the error nicely:

```
==2212== Invalid read of size 4
==2212==    at 0x4000850: main (temp.c:9)
==2212==  Address 0x4aae044 is 4 bytes inside a block of size 40 free'd
==2212==    at 0x48A86B8: free (vg_replace_malloc.c:990)
==2212==    by 0x4000847: main (temp.c:7)
==2212==  Block was alloc'd at
==2212==    at 0x48AC860: calloc (vg_replace_malloc.c:1678)
==2212==    by 0x400083B: main (temp.c:5)
==2212==
```

### Now pass a funny value to `free` (e.g., a pointer in the middle of the array you allocated above). What happens? Do you need tools to find this type of problem?

The compiler gave a warning and gcc logged this message

```
free(): invalid pointer
```

However, valgrind also was helpful:

```
==2305== Invalid free() / delete / delete[] / realloc()
==2305==    at 0x48A86B8: free (vg_replace_malloc.c:990)
==2305==    by 0x400084B: main (temp.c:7)
==2305==  Address 0x4aae04c is 12 bytes inside a block of size 40 alloc'd
==2305==    at 0x48AC860: calloc (vg_replace_malloc.c:1678)
==2305==    by 0x400083B: main (temp.c:5)
```

### Try out some of the other interfaces to memory allocation. For example, create a simple vector-like data structure and related routines that use `realloc()` to manage the vector. Use an array to store the vectors elements; when a user adds an entry to the vector, use `realloc()` to allocate more space for it. How well does such a vector perform? How does it compare to a linked list? Use valgrind to help you find bugs.

I don't want to write a whole test suite for this so I will just try to reason about it. Given the memory structure:

```c
struct arr {
    size_t len;
    size_t capacity;
    int * nums;
}
```

which re-sizes everytime `len >= capacity * .8` I would expect the performance relative to a linked list the be the following:

- Insertion would be worse. It's `O(1)` in most cases but becomes `O(n)` whenever you reach the resize condition. This can be amoritized based on how big the resize is but it is worse than the constant time insertion of linked lists regardless. 
- Lookup would either be better or the same. You could retrive an element based on it's index in constant time but you would need the same search time as a linked list otherwise. Also, checking the length of the list would be constant time whereas it is linear on the linked list. 
- The new data structure would use less memory per element. 


### Spend more time and read about using `gdb` and `valgrind`. Knowing your tools is critical; spend the time and learn how to become an expert debugger in the UNIX and C environment.