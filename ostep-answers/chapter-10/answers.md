### To start things off, let’s learn how to use the simulator to study how to build an effective multi-processor scheduler. The first simulation will run just one job, which has a run-time of 30, and a working-set size of 200. Run this job (called job ’a’ here) on one simulated CPU as follows: `./multi.py -n 1 -L a:30:200`. How long will it take to complete? Turn on the `-c` flag to see a final answer, and the `-t` flag to see a tick-by-tick trace of the job and how it is scheduled.

It would take 10 time units to warm the CPU and then it will run at double speed for the remaining time, meaning it would need 10 more units to finish, totaling 20 time units. However, the deafult cache size is 1-- and this job has a working set of size 200 so I don't think the cache can ever warm, meaning it will need the full 30 time units. 

```
jamesdaniel@MacBook-Pro-3 cpu-sched-multi % python3 multi.py -n 1 -L a:30:200 -c
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:30:200
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 1
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 100
ARG random_order False
ARG trace False
ARG trace_time False
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:30 working_set_size:200

Scheduler central queue: ['a']


Finished time 30

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 0.00 ]
```

### Now increase the cache size so as to make the job’s working set (size=200) fit into the cache (which, by default, is size=100); for example, run `./multi.py -n 1 -L a:30:200 -M 300`. Can you predict how fast the job will run once it fits in cache? (hint: remember the key parameter of the warm rate, which is set by the `-r` flag) Check your answer by running with the solve flag (`-c`) enabled.

I believe the job should run in 20 time units for reasons described above. 

```
jamesdaniel@MacBook-Pro-3 cpu-sched-multi % python3 multi.py -n 1 -L a:30:200 -M 300 -c
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:30:200
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 1
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 300
ARG random_order False
ARG trace False
ARG trace_time False
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:30 working_set_size:200

Scheduler central queue: ['a']


Finished time 20

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 50.00 ]
```

### One cool thing about `multi.py` is that you can see more detail about what is going on with different tracing flags. Run the same simulation as above, but this time with time left tracing enabled (`-T`). This flag shows both the job that was scheduled on a CPU at each time step, as well as how much run-time that job has left after each tick has run. What do you notice about how that second column decreases?

Once the cache is warm the time remianing starts dropping by 2 per step instead of 1. 

### Now add one more bit of tracing, to show the status of each CPU cache for each job, with the `-C` flag. For each job, each cache will either show a blank space (if the cache is cold for that job) or a ’w’ (if the cache is warm for that job). At what point does the cache become warm for job ’a’ in this simple example? What happens as you change the warmup time parameter (`-w`) to lower or higher values than the default?

The cache becomes warm after a job has run on it for 10 consecutive time blocks. If we increase or decrease the warmup time, it takes more or fewer time blocks for the cache to become warm.

### At this point, you should have a good idea of how the simulator works for a single job running on a single CPU. But hey, isn’t this a multi-processor CPU scheduling chapter? Oh yeah! So let’s start working with multiple jobs. Specifically, let’s run the following three jobs on a two-CPU system (i.e., type `./multi.py -n 2 -L a:100:100,b:100:50,c:100:50`) Can you predict how long this will take, given a round-robin centralized scheduler? Use `-c` to see if you were right, and then dive down into details with `-t` to see a step-by-step and then -C to see whether caches got warmed effectively for these jobs. What do you notice?

I believe it will take 150 time blocks. None of the caches will ever get warm with these jobs using round robin scheduling, so we have 300 total time blocks split between two CPUs.

```
jamesdaniel@MacBook-Pro-3 cpu-sched-multi % python3 multi.py -n 2 -L a:100:100,b:100:50,c:100:50 -c
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:100:100,b:100:50,c:100:50
ARG affinity
ARG per_cpu_queues False
ARG num_cpus 2
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 100
ARG random_order False
ARG trace False
ARG trace_time False
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:100 working_set_size:100
Job name:b run_time:100 working_set_size:50
Job name:c run_time:100 working_set_size:50

Scheduler central queue: ['a', 'b', 'c']


Finished time 150

Per-CPU stats
  CPU 0  utilization 100.00 [ warm 0.00 ]
  CPU 1  utilization 100.00 [ warm 0.00 ]
```

### Now we’ll apply some explicit controls to study cache affinity, as described in the chapter. To do this, you’ll need the `-A` flag. This flag can be used to limit which CPUs the scheduler can place a particular job upon. In this case, let’s use it to place jobs ’b’ and ’c’ on CPU 1, while restricting ’a’ to CPU 0. This magic is accomplished by typing this `./multi.py -n 2 -L a:100:100,b:100:50,c:100:50 -A a:0,b:1,c:1` ; don’t forget to turn on various tracing options to see what is really happening! Can you predict how fast this version will run? Why does it do better? Will other combinations of ’a’, ’b’, and ’c’ onto the two processors run faster or slower?

I believe this will run in 110 time units. It does better because pinning the jobs to a given CPU allows the cache to warm up and then the job starts running at double speed. Other combinations of this will not do better because the only way to have a warm cache for all three jobs is to have job a on one CPU and jobs b and c on the other. 

```
jamesdaniel@MacBook-Pro-3 cpu-sched-multi % python3 multi.py -n 2 -L a:100:100,b:100:50,c:100:50 -A a:0,b:1,c:1 -c
ARG seed 0
ARG job_num 3
ARG max_run 100
ARG max_wset 200
ARG job_list a:100:100,b:100:50,c:100:50
ARG affinity a:0,b:1,c:1
ARG per_cpu_queues False
ARG num_cpus 2
ARG quantum 10
ARG peek_interval 30
ARG warmup_time 10
ARG cache_size 100
ARG random_order False
ARG trace False
ARG trace_time False
ARG trace_cache False
ARG trace_sched False
ARG compute True

Job name:a run_time:100 working_set_size:100
Job name:b run_time:100 working_set_size:50
Job name:c run_time:100 working_set_size:50

Scheduler central queue: ['a', 'b', 'c']


Finished time 110

Per-CPU stats
  CPU 0  utilization 50.00 [ warm 40.91 ]
  CPU 1  utilization 100.00 [ warm 81.82 ]
```


### One interesting aspect of caching multiprocessors is the opportunity for better-than-expected speed up of jobs when using multiple CPUs (and their caches) as compared to running jobs on a single processor. Specifically, when you run on N CPUs, sometimes you can speed up by more than a factor of N, a situation entitled super-linear speedup. To experiment with this, use the job description here (`-L a:100:100,b:100:100,c:100:100`) with a small cache (`-M 50`) to create three jobs. Run this on systems with 1, 2, and 3 CPUs (`-n 1, -n 2, -n 3`). Now, do the same, but with a larger per-CPU cache of size 100. What do you notice about performance as the number of CPUs scales? Use `-c` to confirm your guesses, and other tracing flags to dive even deeper.

My guess were right. We bascially see linear speed up for all combinations until we use the large cache and three processors. This setup finally allows us to run all jobs simultaneously and keep a warm cache, taking time from 150 with -n 2 and -M 100 and 55 with -n 3. 

### One other aspect of the simulator worth studying is the per-CPU scheduling option, the `-p` flag. Run with two CPUs again, and this three job configuration (`-L a:100:100,b:100:50,c:100:50`). How does this option do, as opposed to the hand-controlled affinity limits you put in place above? How does performance change as you alter the ’peek interval’ (`-P`) to lower or higher values? How does this per-CPU approach work as the number of CPUs scales?

This is a bit faster for these jobs because it lets the first CPU grab the second job from the second CPU after it finishes with job A. I can get marginal improvements out of the runitme by shortening the peeek interval. As far as I can tell, the simulator does not assign any cost to checking other cpus for jobs to take so we can drop the peak interval to 1 and not see any real problems. If there are ever more CPUs than jobs this kind of becomes a mess because CPUs will take jobs from CPUs that have a warm queue and slow down the overall runtime. 

### Finally, feel free to just generate random workloads and see if you can predict their performance on different numbers of processors, cache sizes, and scheduling options. If you do this, you’ll soon be a multi-processor scheduling master, which is a pretty awesome thing to be. Good luck!

