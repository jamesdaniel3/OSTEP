### Examine `flag.s`. This code “implements” locking with a single memory flag. Can you understand the assembly?

### When you run with the defaults, does `flag.s` work? Use the `-M` and `-R` flags to trace variables and registers (and turn on `-c` to see their values). Can you predict what value will end up in flag?

With default values it works, but if you change the interrupt frequency (or `bx`), the lock doesn't really function and it will break. 

###  Change the value of the register `%bx` with the `-a` flag (e.g., `-a bx=2,bx=2` if you are running just two threads). What does the code do? How does it change your answer for the question above?

This register just determines how many times the register is going to attempt to update the flag.

### Set `bx` to a high value for each thread, and then use the `-i` flag to generate different interrupt frequencies; what values lead to a bad outcomes? Which lead to good outcomes?

It seems that 11 and 15 (and there multiples) are the interrupts that work. 11 makes sense becuase the program has 11 instructions not including the final halt. I have hard time intuiting why 15 works. It's a full loop of the program followed by a check for the flag but otherwise I'm lost. 

### Now let’s look at the program `test-and-set.s`. First, try to understand the code, which uses the `xchg` instruction to build a simple locking primitive. How is the lock acquire written? How about lock release?

`xchg` loads the value of a regsiter into memory (our mutex) and returns the value that was previously in memory, so to acquire the lock we run `xchg` with a register containing 1 and our mutex. If the register contains 0 after, the lock is acquired and we proceed. If the register contains 1 after, the lock was already held by another thread and we loop. The lock release just sets the mutex equal to 0. 

### Now run the code, changing the value of the interrupt interval (`-i`) again, and making sure to loop for a number of times. Does the code always work as expected? Does it sometimes lead to an inefficient use of the CPU? How could you quantify that?

The code always works as expected in this case. Yes, it does sometimes lead to an inefficient use of the CPU. If the interrupt causes a thread to stop right before it releases the mutex, than the second thread will spend it's entire cycle spinning. If there were many threads than every thread would just spin until the original thread was able to run again and release the lock. 

### Use the `-P` flag to generate specific tests of the locking code. For example, run a schedule that grabs the lock in the first thread, but then tries to acquire it in the second. Does the right thing happen? What else should you test?

The right thing happens. It also makes sense to have the first thread make the `xchg` instruction and halt there while hte second thread gets to make an attempt at aquiring the lock. 

### Now let’s look at the code in `peterson.s`, which implements Peterson’s algorithm (mentioned in a sidebar in the text). Study the code and see if you can make sense of it.

### Now run the code with different values of `-i`. What kinds of different behavior do you see? Make sure to set the thread IDs appropriately (using `-a bx=0,bx=1` for example) as the code assumes it.

It doesn't break with any interrupt value. 

### Can you control the scheduling (with the `-P` flag) to “prove” that the code works? What are the different cases you should show hold? Think about mutual exclusion and deadlock avoidance.

You should check that the second thread cannot aquire the lock when the first thread already has it and that both threads cannot aquire the lock / fall out of spin simultaneously. I'm not sure how you would check for deadlock avoidance. I guess you should check to make sure that if one thread is already locked than the other will not get stuck in the spin.

### Now study the code for the ticket lock in `ticket.s`. Does it match the code in the chapter? Then run with the following flags: `-a bx=1000,bx=1000` (causing each thread to loop through the critical section 1000 times). Watch what happens; do the threads spend much time spin-waiting for the lock?

It looks like it pretty much matches the code in the chapter. It definelty behaves the same. The threads spend a bunch of time spin-waiting for locks. Because they go through one iteration and then immediately increment the turn value. If any other thread has the ticket for the next turn, than they just spin for the rest of their processor time. 

### How does the code behave as you add more threads?

I feel like there should be even more time spinning but it kinda seems like it's the same as before. 

### Now examine `yield.s`, in which a yield instruction enables one thread to yield control of the CPU (realistically, this would be an OS primitive, but for the simplicity, we assume an instruction does the task). Find a scenario where `test-and-set.s` wastes cycles spinning, but `yield.s` does not. How many instructions are saved? In what scenarios do these savings arise?

`yield.s` will save cycles as compared to `test-and-set.s` basically any time a thread tries to access the lock while another thread has access to it. The number of instructions saved is tied to how long it takes for the interrupt to occur, how many threads there are, and how long it takes for the scheduler to run the thread that needs to release the lock. 

But basically, all of the cycles that the blocked thread would have spun for are saved by yielding the thread if the next thread scheduled runs and releases the lock.


### Finally, examine `test-and-test-and-set.s`. What does this lock do? What kind of savings does it introduce as compared to `test-and-set.s`?

It saves us from having to run the `xchg` instruction repeatedly by just looping based on the mutex value. If the value stored in the mutex is not zero we stay in the loop. Once the mutex is free, than we run the atomic swap with a check to make sure there wasn't a race condition. 

This is more efficient because the `xchg` instruction invloves loading a value into a register, than storing the value in memory temporarily, moving that registers value to memory, and moving that temp value into the register. Instead of doing that every loop, we can now just load the memory of mutex and compare it to 0. This saves us a few instructions per cycle. 