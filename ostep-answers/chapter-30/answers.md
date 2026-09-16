###  Our first question focuses on `main-two-cvs-while.c` (the working solution). First, study the code. Do you think you have an understanding of what should happen when you run the program?

Yes, this is the working example from the chapter. It basically has a producer and a consumer. They each wait on one condition variable and alert based on another. There is a shared buffer that they can write to and read from. 

### Run with one producer and one consumer, and have the producer produce a few values. Start with a buffer (size 1), and then increase it. How does the behavior of the code change with larger buffers? (or does it?) What would you predict `num_full` to be with different buffer sizes (e.g., `-m 10`) and different numbers of produced items (e.g., `-l 100`), when you change the consumer sleep string from default (no sleep) to `-C 0,0,0,0,0,0,1`?

The behavior doesn't seem to change over regardless of the value of `m` or `l`. This is because the consumer seems to by default wake up every time the producer tries to loop (the inverse is also likely true.) However, if I force the consumer to stay asleep, I make the the producer write up to `m` values.

### If possible, run the code on different systems (e.g., a Mac and Linux). Do you see different behavior across these systems?

I am not seeing different behavior between my mac and linux machines. 

### Let’s look at some timings. How long do you think the following execution, with one producer, three consumers, a single-entry shared buffer, and each consumer pausing at point `c3` for a second, will take? `./main-two-cvs-while -p 1 -c 3 -m 1 -C 0,0,0,1,0,0,0:0,0,0,1,0,0,0:0,0,0,1,0,0,0 -l 10 -v -t`

It ended up taking about 13.2 seconds. I have no idea how I was supposed to be able to figure that out; maybe they just wanted me to make an educated guess?

### Now change the size of the shared buffer to 3 (`-m 3`). Will this make any difference in the total time?

I think this will decrease the total time because the producer will fill this up and all the consumers will be able to take from it more effectively.

---

It ended up being quicker but seemingly not for the reasons that I thought. the shared buffer never has more than 1 value in it so it's not like the producer is writing a bunch of values at once. In fact, the slower implementation only has two more lines in the verbose logging. Maybe the larger list is quicker because the producer never has to wait on a condition variable? When it inserts a value once and then the consumer takes over, the it did not begin waiting, and it does not have to start waiting until it reaches a full list, which never happens because the consumers seem to always get to execute before it can produce consecutive values. This removes and delay in waking it up. However, it's hard to believe that would make up for a second on 10 entries.

### Now change the location of the sleep to `c6` (this models a consumer taking something off the queue and then doing something with it), again using a single-entry buffer. What time do you predict in this case? `./main-two-cvs-while -p 1 -c 3 -m 1 -C 0,0,0,0,0,0,1:0,0,0,0,0,0,1:0,0,0,0,0,0,1 -l 10 -v -t`

I predict that this will be faster than the original case because the consumers will sleep at a time when they likely would have had to sleep anway. This ended up being much faster, seemingly because consumers weren't being interrupted at bad times. 

### Finally, change the buffer size to 3 again (`-m 3`). What time do you predict now?

I think this should improve the speed for the same reasons as above but I'm not sure I will see any change. 

---

There was no clear change in time despite actually seeing the shared buffer fill with more than one value this time. 

### Now let’s look at `main-one-cv-while.c`. Can you configure a sleep string, assuming a single producer, one consumer, and a buffer of size 1, to cause a problem with this code?

There's no way to break this with the above constraints. The bug in this code can only be triggered when we have multiple consumers or multiple producers. 

### Now change the number of consumers to two. Can you construct sleep strings for the producer and the consumers so as to cause a problem in the code?

I am having trouble constructing the strings. This is how it's supposed to work: both consumer run; neither can consume so they both go to sleep. The producer runs, inputs, and goes to sleep. One consumer runs, consumes, and sends an alert to wake up. The signal wakes up the sleeping consumer, which goes back to sleep. The producer is never able to wake up agian to keep producing. 

### Now examine `main-two-cvs-if.c`. Can you cause a problem to happen in this code? Again consider the case where there is only one consumer, and then the case where there is more than one.

This is enough to cause the problem if the scheduler does a bit of work:

`./main-two-cvs-if -c 2 -v -C 0,0,1,0,0,0:0,0,0,0,0,0`. Again, this bug cannot be cause without two consumers, but basically if a consumer is interrupted between the time when it checks if it can run and then time when it runs by a seperate consumer which then runs, there wil be a failure. 

### Finally, examine `main-two-cvs-while-extra-unlock.c`. What problem arises when you release the lock before doing a put or a get? Can you reliably cause such a problem to happen, given the sleep strings? What bad thing can happen?

It seems to me that the problem is that a second consumer or producer will run if you fall and if you fall asleep with out the lock and wake up and try to execute than you will execute on bad data. 