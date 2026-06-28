### Run a few randomly-generated problems with just two jobs and two queues; compute the MLFQ execution trace for each. Make your life easier by limiting the length of each job and turning off I/Os.

Done

### How would you run the scheduler to reproduce each of the examples in the chapter?

Example 1: A single long-running job
```
python3 mlfq.py -l 0,200,0 -q 20 -S
```

Example 2: Along came a short job
```
python3 mlfq.py -l 0,200,0:100,20,0 -q 10 -S
```

Example 3: What about I/O?
```
python3 mlfq.py -l 0,200,0:50,25,1 -q 10 -S
```

### How would you configure the scheduler parameters to behave just like a round-robin scheduler?

If it only has one queue than everything will be on the same level and subject to round robing scheduling:

```
python3 mlfq.py -n 1
```

### Craft a workload with two jobs and scheduler parameters so that one job takes advantage of the older Rules 4a and 4b (turned on with the -S flag) to game the scheduler and obtain 99% of the CPU over a particular time interval.

```
python3 mlfq.py -l 0,1000,0:0,1000,99 -q 100 -S -i 1
```

### Given a system with a quantum length of 10 ms in its highest queue, how often would you have to boost jobs back to the highest priority level (with the -B flag) in order to guarantee that a single longrunning (and potentially-starving) job gets at least 5% of the CPU?

I don't think this can be guaranteed. What if there were 50 jobs? Than no matter how often the job got boosted to the top it couldn't get more than 2% of the CPU. However, assuming this is just a system with many short jobs filtering in, I think you just have to get the job in every 20 evaluations, so if you boosed the queue every 200 ms you would guarantee that the job gets at least 5% cpu usage. 

### One question that arises in scheduling is which end of a queue to add a job that just finished I/O; the -I flag changes this behavior for this scheduling simulator. Play around with some workloads and see if you can see the effect of this flag

I was able to use this flag to speed up the total time to run in an example where there was one job that needed to do a lot of IO. This let that job continuosly take priority and the other jobs could fill in the space it used to do the IO. This could cause issues if there were many IO jobs in the queue. If the IO time is not long enough for non IO-heavy jobs to get picked up by the queue and run while other jobs are awaiting IO, they could starve as the IO jobs just are constantly pushed ahead of them in the queue. 