### Let’s examine a simple program, `loop.s`. First, just read and understand it. Then, run it with these arguments (`./x86.py -t 1 -p loop.s -i 100 -R dx`) This specifies a single thread, an interrupt every 100 instructions, and tracing of register `%dx`. What will `%dx` be during the run? Use the `-c` flag to check your answers; the answers, on the left, show the value of the register (or memory value) after the instruction on the right has run.

The register will start at 0, then decrement to -1 

### Same code, different flags: (`./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx`) This specifies two threads, and initializes each `%dx` to 3. What values will `%dx` see? Run with `-c` to check. Does the presence of multiple threads affect your calculations? Is there a race in this code?

3 -> -1, there is no race in the code because each thread get's its own registers; also, the interrupts are only happenign every 100 instructions so this code won't execute for long enough to cause an interrupt 

### Run this: `./x86.py -p loop.s -t 2 -i 3 -r -R dx -a dx=3,dx=3` This makes the interrupt interval small/random; use different seeds (`-s`) to see different interleavings. Does the interrupt frequency change anything?

The interrupt changes how often things are interrupted. However, because each thread has it's own registers and this operates only on the registers, there can be no race condition. 

### Now, a different program, `looping-race-nolock.s`, which accesses a shared variable located at address 2000; we’ll call this variable value. Run it with a single thread to confirm your understanding: `./x86.py -p looping-race-nolock.s -t 1 -M 2000` What is value (i.e., at memory address 2000) throughout the run? Use `-c` to check.

Assuming everything without an explicit intialization starts at 0 than the value at 2000 will be 0 and then 1 and then the loop will stop. 

### Run with multiple iterations/threads: `./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000` Why does each thread loop three times? What is final value of value?

Each loop threads three times because the value of bx is set to three and that is the register that is decremented to create the loop. The final value will be 6. This code could have a race condition but the default interrupt schedule is every 100 instructions which is big enough that both threads will get to execute completely without interruption. 

### Run with random interrupt intervals: `./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0` with different seeds (`-s 1`, `-s 2`, etc.) Can you tell by looking at the thread interleaving what the final value of value will be? Does the timing of the interrupt matter? Where can it safely occur? Where not? In other words, where is the critical section exactly?

Yes, you can tell based on the interleaving what the final value will be. The timing of the interrupt does matter. IF the interrupt occurs after `mov 2000, %ax` but before `mov %ax, 2000`, than the final count will be incorrect. The critical section is this:

```
mov 2000, %ax  # get 'value' at address 2000
add $1, %ax    # increment it
mov %ax, 2000  # store it back
```

### Now examine fixed interrupt intervals: `./x86.py -p looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1` What will the final value of the shared variable value be? What about when you change `-i 2`, `-i 3`, etc.? For which interrupt intervals does the program give the “correct” answer?

i = 1: 1
i = 2: 1
i = 3: 2

In the above options, interval interrupts at 3 works. I believe any interrupt that is a multiple of 3 would work 

### Run the same for more loops (e.g., set `-a bx=100`). What interrupt intervals (`-i`) lead to a correct outcome? Which intervals are surprising?

See the baove answer. None of the intervals really surprsied me as it seems to follow a pattern where multiples of 3 work but multiples of 2 or prime numbers do not. 

### One last program: `wait-for-me.s`. Run: `./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000` This sets the `%ax` register to 1 for thread 0, and 0 for thread 1, and watches `%ax` and memory location 2000. How should the code behave? How is the value at location 2000 being used by the threads? What will its final value be?

The value at location 2000 is used as a flag for the signaller being encountered, meaning that the value 1 has been found in the `%ax`. If the value is found it puts the value 1 in address 2000. If the value is not found, the waiter loop is entered and it just repeatedly checks the value at 2000 and exits the loop and halts when it is 1. Given the above execution, the final value of 2000 will be 1. Because the memory is shared, one thread can update it while the second waits. 

### Now switch the inputs: `./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000` How do the threads behave? What is thread 0 doing? How would changing the interrupt interval (e.g., `-i 1000`, or perhaps to use random intervals) change the trace outcome? Is the program efficiently using the CPU?

Thread 0 is now the waiter, if there are no interrupts and it runs first than it will run infinitely (assuming 1 CPU). Changing the interrupt would change how long it takes for the signler to signal assuming it runs second. The program is not efficiently using CPU. The alerting thread can exit as soon as it alerts, so what should happen is a thread should be interrupted as soon as it is not found to be the alerting thread, than the alerting thread can run, than the original thread can poll memory. 