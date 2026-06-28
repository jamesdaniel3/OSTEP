### Compute the response time and turnaround time when running three jobs of length $200$ with the `SJF` and `FIFO` schedulers.

The average turnaround time for both policies will be $400$. The average response time for both policies will be $200$. There is no difference in this case because all of the jobs have the same length, so `SJF` just becomes `FIFO`.

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p FIFO -c -l 200,200,200
...
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 200.00  Wait 0.00
  Job   1 -- Response: 200.00  Turnaround 400.00  Wait 200.00
  Job   2 -- Response: 400.00  Turnaround 600.00  Wait 400.00

  Average -- Response: 200.00  Turnaround 400.00  Wait 200.00
```

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p SJF -c -l 200,200,200
...
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 200.00  Wait 0.00
  Job   1 -- Response: 200.00  Turnaround 400.00  Wait 200.00
  Job   2 -- Response: 400.00  Turnaround 600.00  Wait 400.00

  Average -- Response: 200.00  Turnaround 400.00  Wait 200.00
```

###  Now do the same but with jobs of different lengths: $100$, $200$, and $300$.

The average turnaround time for both policies will be $333.33$. The average response time for both policies will be $133.33$. There is no difference in this case because the jobs arrived ordered from shortest to longest, so `SJF` and `FIFO` result in the same schedule.

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p FIFO -c -l 100,200,300
...
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   1 -- Response: 100.00  Turnaround 300.00  Wait 100.00
  Job   2 -- Response: 300.00  Turnaround 600.00  Wait 300.00

  Average -- Response: 133.33  Turnaround 333.33  Wait 133.33
```

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p SJF -c -l 100,200,300
...
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   1 -- Response: 100.00  Turnaround 300.00  Wait 100.00
  Job   2 -- Response: 300.00  Turnaround 600.00  Wait 300.00

  Average -- Response: 133.33  Turnaround 333.33  Wait 133.33
```

### Now do the same, but also with the `RR` scheduler and a time-slice of $1$.

On the first set of inputs (3 jobs with a length of 200), the `RR` scheduler results in a response time of $1$ with a turnaround time of $599$. On the second set of inputs (100, 200, 300), we response time remains $1$, but the turaround time is  $465.6$.

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p RR -c -l 200,200,200
...
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 598.00  Wait 398.00
  Job   1 -- Response: 1.00  Turnaround 599.00  Wait 399.00
  Job   2 -- Response: 2.00  Turnaround 600.00  Wait 400.00

  Average -- Response: 1.00  Turnaround 599.00  Wait 399.00
```

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p RR -c -l 100,200,300
...
Final statistics:
  Job   0 -- Response: 0.00  Turnaround 298.00  Wait 198.00
  Job   1 -- Response: 1.00  Turnaround 499.00  Wait 299.00
  Job   2 -- Response: 2.00  Turnaround 600.00  Wait 300.00

  Average -- Response: 1.00  Turnaround 465.67  Wait 265.67
```


### For what types of workloads does `SJF` deliver the same turnaround times as `FIFO`?

`SJF` delivers the same turnaround times as `FIFO` if the jobs all have the same length or if they arrive in order from shortest to longest. 

### For what types of workloads and quantum lengths does `SJF` deliver the same response times as `RR`?

The job length must be equal to the quantum length and all jobs must be the same length for `SJF` to have the same response time as `RR`. 

### What happens to response time with `SJF` as job lengths increase? Can you use the simulator to demonstrate the trend?

Response time will increase as job lengths increase. The only way this would not increase response time is if the longest job had an increase in length. Here are a few examples:

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p SJF -c -l 200,300,100
...
Final statistics:
  Job   2 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   0 -- Response: 100.00  Turnaround 300.00  Wait 100.00
  Job   1 -- Response: 300.00  Turnaround 600.00  Wait 300.00

  Average -- Response: 133.33  Turnaround 333.33  Wait 133.33
```

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p SJF -c -l 200,350,100
...
Final statistics:
  Job   2 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   0 -- Response: 100.00  Turnaround 300.00  Wait 100.00
  Job   1 -- Response: 300.00  Turnaround 650.00  Wait 300.00

  Average -- Response: 133.33  Turnaround 350.00  Wait 133.33
```

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p SJF -c -l 250,300,100
...
Final statistics:
  Job   2 -- Response: 0.00  Turnaround 100.00  Wait 0.00
  Job   0 -- Response: 100.00  Turnaround 350.00  Wait 100.00
  Job   1 -- Response: 350.00  Turnaround 650.00  Wait 350.00

  Average -- Response: 150.00  Turnaround 366.67  Wait 150.00
```

```
jamesdaniel@MacBook-Pro-3 cpu-sched % python3 scheduler.py -p SJF -c -l 200,300,150
...
Final statistics:
  Job   2 -- Response: 0.00  Turnaround 150.00  Wait 0.00
  Job   0 -- Response: 150.00  Turnaround 350.00  Wait 150.00
  Job   1 -- Response: 350.00  Turnaround 650.00  Wait 350.00

  Average -- Response: 166.67  Turnaround 383.33  Wait 166.67
```


### What happens to response time with `RR` as quantum lengths increase? Can you write an equation that gives the worst-case response time, given `N` jobs?

Response time grows proportionally to the quantum length. 

Where `q` is quantum length: $\frac{q * \sum_{i=0}^{N} N}{N}$ 