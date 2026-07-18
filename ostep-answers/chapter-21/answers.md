### First, open two separate terminal connections to the same machine, so that you can easily run something in one window and the other. Now, in one window, run `vmstat 1`, which shows statistics about machine usage every second. Read the man page, the associated README, and any other information you need so that you can understand its output. Leave this window running `vmstat` for the rest of the exercises below. Now, we will run the program `mem.c` but with very little memory usage. This can be accomplished by typing `./mem 1` (which uses only 1 MB of memory). How do the CPU usage statistics change when running `mem`? Do the numbers in the user time column make sense? How does this change when running more than one instance of `mem` at once?

Yes, the numbers make sense. For context, I am running on a linux VM with 8GB of memory and 2 CPUs. When I run one process, `vmstat` reports that 50% of all CPU time is used by user programs. When I run 2 processes, that number goes to 100. I moved up to three processes to see if I could get the number to drop below 100 for the sake of context switching but I am not seeing that happen.

### Let’s now start looking at some of the memory statistics while running mem. We’ll focus on two columns: swpd (the amount of virtual memory used) and free (the amount of idle memory). Run `./mem 1024` (which allocates 1024 MB) and watch how these values change. Then kill the running program (by typing control-c) and watch again how the values change. What do you notice about the values? In particular, how does the free column change when the program exits? Does the amount of free memory increase by the expected amount when mem exits?

Interestingly the amount of free memory drop by slightly less than the amount of memory the program requests. It drops by at least 1000 MB but it does not drop by 1024 MB. I think this is because the OS reclaimed some memroy when running the program. 

### We’ll next look at the swap columns (si and so), which indicate how much swapping is taking place to and from the disk. Of course, to activate these, you’ll need to run mem with large amounts of memory. First, examine how much free memory is on your Linux system (for example, by typing `cat /proc/meminfo`; type `man proc` for details on the `/proc` file system and the types of information you can find there). One of the first entries in `/proc/meminfo` is the total amount of memory in your system. Let’s assume it’s something like 8 GB of memory; if so, start by running `mem 4000` (about 4 GB) and watching the swap in/out columns. Do they ever give non-zero values? Then, try with 5000, 6000, etc. What happens to these values as the program enters the second loop (and beyond), as compared to the first loop? How much data (total) are swapped in and out during the second, third, and subsequent loops? (do the numbers make sense?)

I did not reliably see swaps happening until ~ 8 GB. The first loop executes way faster than subsequent loops. This makes sense because the first loop does not have to read from disk for the majority of it's iteration. The first ~6.5 GB of data get to just be writting without any swapping. The future loops have to do much more I/O due to already used memory. 

### Do the same experiments as above, but now watch the other statistics (such as CPU utilization, and block I/O statistics). How do they change when `mem` is running?

When `mem` is running with smaller values we pretty much see the cpu spend all of its time running non-kernel code and not reading from I/O at all and never waiting. When I run `mem` with large values like 8 GB, the system is doing a ton of reading and writing to the disk and it has to spend nearly 10x the time running kernel code as it does writing non-kernel code, and it spends even more time idling waiting for I/O. It may not be idle if there were other processes for it to context switch to but it would still have to wait. 

### Now let’s examine performance. Pick an input for `mem` that comfortably fits in memory (say 4000 if the amount of memory on the system is 8 GB). How long does loop 0 take (and subsequent loops 1, 2, etc.)? Now pick a size comfortably beyond the size of memory (say 12000 again assuming 8 GB of memory). How long do the loops take here? How do the bandwidth numbers compare? How different is performance when constantly swapping versus fitting everything comfortably in memory? Can you make a graph, with the size of memory used by mem on the x-axis, and the bandwidth of accessing said memory on the y-axis? Finally, how does the performance of the first loop compare to that of subsequent loops, for both the case where everything fits in memory and where it doesn’t?

Easy Run:
```
james@james-linux-vm:~$ ./mem 4000
allocating 4194304000 bytes (4000.00 MB)
  number of integers in array: 1048576000
loop 0 in 555.92 ms (bandwidth: 7195.24 MB/s)
loop 1 in 302.40 ms (bandwidth: 13227.52 MB/s)
loop 2 in 243.73 ms (bandwidth: 16411.74 MB/s)
loop 3 in 244.35 ms (bandwidth: 16369.90 MB/s)
loop 4 in 243.93 ms (bandwidth: 16398.22 MB/s)
```

Hard Run:
```
james@james-linux-vm:~$ ./mem 8000
allocating 8388608000 bytes (8000.00 MB)
  number of integers in array: 2097152000
loop 0 in 1236.75 ms (bandwidth: 6468.58 MB/s)
loop 1 in 13042.13 ms (bandwidth: 613.40 MB/s)
loop 2 in 10336.39 ms (bandwidth: 773.96 MB/s)
loop 3 in 12903.87 ms (bandwidth: 619.97 MB/s)
loop 4 in 16218.41 ms (bandwidth: 493.27 MB/s)
```

Run that died:
```
james@james-linux-vm:~$ ./mem 9000
allocating 9437184000 bytes (9000.00 MB)
  number of integers in array: 2359296000
loop 0 in 1808.11 ms (bandwidth: 4977.58 MB/s)
loop 1 in 9871.48 ms (bandwidth: 911.72 MB/s)
loop 2 in 11589.71 ms (bandwidth: 776.55 MB/s)
Killed                     ./mem 9000
```

It looks like when we have to constantly swap to fit all of our data, the bandwith is roughly 40x slower. In the test where the data fits comfortably in memory. The first run is the slowest and subsequent runs are 2x+ faster. In the case where the data does not fit in memory, the first loop is the fastest and subsequent runs are 5-10x slower. I'm not making the graph. 

### Swap space isn’t infinite. You can use the tool `swapon` with the `-s` flag to see how much swap space is available. What happens if you try to run mem with increasingly large values, beyond what seems to be available in swap? At what point does the memory allocation fail?

`swapon` says that I have 2GB of available space. earlier commands like `cat /proc/meminfo` said that my image had about 6.5 GB of free space, so as we appraoch 8500 MB I get worried about the system. Here it died after 7 loops:

```
james@james-linux-vm:~$ ./mem 8500
allocating 8912896000 bytes (8500.00 MB)
  number of integers in array: 2228224000
loop 0 in 1410.89 ms (bandwidth: 6024.55 MB/s)
loop 1 in 12209.81 ms (bandwidth: 696.16 MB/s)
loop 2 in 12825.60 ms (bandwidth: 662.74 MB/s)
loop 3 in 9772.02 ms (bandwidth: 869.83 MB/s)
loop 4 in 12173.33 ms (bandwidth: 698.25 MB/s)
loop 5 in 14852.37 ms (bandwidth: 572.30 MB/s)
loop 6 in 18480.88 ms (bandwidth: 459.93 MB/s)
Killed                     ./mem 8500
```

earlier with 9GB it died after 3 and with 10GB it could not complete a loop

### Finally, if you’re advanced, you can configure your system to use different swap devices using `swapon` and `swapoff`. Read the man pages for details. If you have access to different hardware, see how the performance of swapping changes when swapping to a classic hard drive, a flash-based SSD, and even a RAID array. How much can swapping performance be improved via newer devices? How close can you get to in-memory performance?

I do not have access to different hardware