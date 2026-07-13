### For timing, you’ll need to use a timer (e.g., gettimeofday()). How precise is such a timer? How long does an operation have to take in order for you to time it precisely? (this will help determine how many times, in a loop, you’ll have to repeat a page access in order to time it successfully)

### Write the program, called `tlb.c`, that can roughly measure the cost of accessing each page. Inputs to the program should be: the number of pages to touch and the number of trials.

### Now write a script in your favorite scripting language (bash?) to run this program, while varying the number of pages accessed from 1 up to a few thousand, perhaps incrementing by a factor of two per iteration. Run the script on different machines and gather some data. How many trials are needed to get reliable measurements?

### Next, graph the results, making a graph that looks similar to the one above. Use a good tool like ploticus or even zplot. Visualization usually makes the data much easier to digest; why do you think that is?

I don't feel like creating a graph:

```
james@james-linux-vm:~$ ./run_tests.sh
1 Pages; 500000000 Trials: 2.000000000000000 Nanoseconds Per Accesss
2 Pages; 500000000 Trials: 1.000000000000000 Nanoseconds Per Accesss
4 Pages; 500000000 Trials: 1.000000000000000 Nanoseconds Per Accesss
8 Pages; 500000000 Trials: 0.750000000000000 Nanoseconds Per Accesss
16 Pages; 500000000 Trials: 0.750000000000000 Nanoseconds Per Accesss
32 Pages; 500000000 Trials: 0.750000000000000 Nanoseconds Per Accesss
64 Pages; 3000000 Trials: 5.208333333333334 Nanoseconds Per Accesss
128 Pages; 3000000 Trials: 5.208333333333334 Nanoseconds Per Accesss
256 Pages; 3000000 Trials: 6.510416666666667 Nanoseconds Per Accesss
512 Pages; 3000000 Trials: 7.161458333333333 Nanoseconds Per Accesss
1024 Pages; 3000000 Trials: 6.835937500000000 Nanoseconds Per Accesss
2048 Pages; 3000000 Trials: 7.161458333333333 Nanoseconds Per Accesss
```

### One thing to watch out for is compiler optimization. Compilers do all sorts of clever things, including removing loops which increment values that no other part of the program subsequently uses. How can you ensure the compiler does not remove the main loop above from your TLB size estimator?

I just compiled with `-O0`, I think we could also use volatile pointers or some other tricks. 

### Another thing to watch out for is the fact that most systems today ship with multiple CPUs, and each CPU, of course, has its own TLB hierarchy. To really get good measurements, you have to run your code on just one CPU, instead of letting the scheduler bounce it from one CPU to the next. How can you do that? (hint: look up “pinning a thread” on Google for some clues) What will happen if you don’t do this, and the code moves from one CPU to the other?

I ran this on a VM with only one vCPU. I am working on a macbook and apple doesn't expose a way to pin threads that I am aware of. I could rent a linux box or boot linux somewhere but I don't want to atm. 

### Another issue that might arise relates to initialization. If you don’t initialize the array a above before accessing it, the first time you access it will be very expensive, due to initial access costs such as demand zeroing. Will this affect your code and its timing? What can you do to counterbalance these potential costs?

This will effect my timing but it will be extremely minimal. If the fist access to each page causes issues than at worse it will cause a problem on 1/3000000 runs which I feel like could amoritize out. We could always interate through all of the pages once before starting the timer to avoid this cost. 