### First let’s make sure you understand how the programs generally work, and some of the key options. Study the code in `vector-deadlock.c`, as well as in `main-common.c` and related files. Now, run `./vector-deadlock -n 2 -l 1 -v`, which instantiates two threads (`-n 2`), each of which does one vector add (`-l 1`), and does so in verbose mode (`-v`). Make sure you understand the output. How does the output change from run to run?

The only way the this program changes between runs is that sometimes the first add completes before the second add starts and sometimes it completes after. 

### Now add the `-d` flag, and change the number of loops (`-l`) from 1 to higher numbers. What happens? Does the code (always) deadlock?

I can actually almost never get the code to deadlock. However, if I add a sleep in between the two lock oeprations to force the scheduler to release the first thread, than I get a deadlock everytime.

### How does changing the number of threads (`-n`) change the outcome of the program? Are there any values of `-n` that ensure no deadlock occurs?

As there are more threads, the likelyhood of a deadlock increases. If you set the program to execute with only one thread, a deadlock cannot occur. 

### Now examine the code in `vector-global-order.c`. First, make sure you understand what the code is trying to do; do you understand why the code avoids deadlock? Also, why is there a special case in this vector_add() routine when the source and destination vectors are the same?

The code works by attempting to acquire locks in the order that the locks are stored in memory, starting with the lock at the lowest memory address. It doesn't actually matter which order the locks are aquired in; the important feature of this is that the order that the locks are aquired in is consistent. The special case is necessary because if the locks are the same and you try to acquire them both, than you will try to acquire the same lock twice and cause deadlock. Interstingly, this is a potential problem for all of these functions but it isn't checked anywhere else. I guess the other reason for having the check is that if the implementation was the same (they didn't use an else) than no locks would be acquired when both args pointed to the same lock.

### Now run the code with the following flags: `-t -n 2 -l 100000 -d`. How long does the code take to complete? How does the total time change when you increase the number of loops, or the number of threads?

Increases in both pretty much increase the time to completion proportionally:

```
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 2 -l 100000 -d
Time: 0.39 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 4 -l 100000 -d
Time: 1.45 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 8 -l 100000 -d
Time: 3.99 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 2 -l 200000 -d
Time: 0.82 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 2 -l 400000 -d
Time: 1.53 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 2 -l 800000 -d
Time: 3.04 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 8 -l 800000 -d
Time: 28.86 seconds
```

### What happens if you turn on the parallelism flag (`-p`)? How much would you expect performance to change when each thread is working on adding different vectors (which is what `-p` enables) versus working on the same ones?

I would expect parallelism to allow us to actually see the benefits of threads. So if we 4x the number of threads I would expect the time to completion to drop by 4x. 

```
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-global-order -t -n 8 -l 800000 -d -p
Time: 0.74 seconds
```

Turns out it was way more effective than that. 

### Now let’s study `vector-try-wait.c`. First make sure you understand the code. Is the first call to `pthread_mutex_trylock()` really needed? Now run the code. How fast does it run compared to the global order approach? How does the number of retries, as counted by the code, change as the number of threads increases?

I actually don't think the first trylock is required. My best guess is that it is used to make the roll back have less overhead. But this kinda seems like it has the same problem as a spin lock so I really don't see what the advantage would be of using this over a normal single trylock implementation. 

```
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-try-wait -t -n 2 -l 100000 -d
Retries: 14775315
Time: 1.96 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-try-wait -t -n 4 -l 100000 -d
Retries: 53532695
Time: 10.16 seconds
james@linux-x86:~/OSTEP/ostep-homework/threads-bugs$ ./vector-try-wait -t -n 8 -l 100000 -d
Retries: 134323857
Time: 38.38 seconds
```

It seems that the number of retries grows faster than the number of threads. 

### Now let’s look at `vector-avoid-hold-and-wait.c`. What is the main problem with this approach? How does its performance compare to the other versions, when running both with `-p` and without it?

It uses one global lock, this makes it impossible for the code to be executed concurrently. I think we will see no benefits from parallelism. 

---

I was wrong about seeing no benefits of parallelism. It turns out the benefits are just decreased. 

### Finally, let’s look at `vector-nolock.c`. This version doesn’t use locks at all; does it provide the exact same semantics as the other versions? Why or why not?

I would say it doesn't provide teh exact same semantics. One example is how in the other examples, the lock is acquired, than all the additions are made, then the lock is released. In the no lock implementation, this is accomplished with atomic instructions, which means there is a gap in between every successive call to fetchAndAdd where something else could in theory execute. For example, I think you could see a difference in behavior when using `vector-nolock.c` twice, once to add vectors A and B and once to add vectors B and C, and using one of the original implementations for the same task. The orignal implementations would have a lock on vector B even when it was the secondary vector, so it could not be changed while it was being used. The nolock implementation makes no such guarantee for the array that is being added from. 

### Now compare its performance to the other versions, both when threads are working on the same two vectors (no `-p`) and when each thread is working on separate vectors (`-p`). How does this no-lock version perform?

It appears to perform similarly to the other implemenatations when we allow for parallelism but greatly outperform when they are working on the same threads, presumably due to the decreased overhead of the locks. 
