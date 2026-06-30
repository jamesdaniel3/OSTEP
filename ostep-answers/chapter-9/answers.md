## Compute the solutions for simulations with 3 jobs and random seeds of 1, 2, and 3.

### Now run with two specific jobs: each of length 10, but one (job 0) with 1 ticket and the other (job 1) with 100 (e.g., `-l 10:1,10:100`). What happens when the number of tickets is so imbalanced? Will job 0 ever run before job 1 completes? How often? In general, what does such a ticket imbalance do to the behavior of lottery scheduling?

Job 0 defintely could run before job 1 completes. How often depends but it's bascially a function of the relative ticket count of each job multiplied by the length of the job. In this case, the odds are $(1 - (100/101)^{10}) * 100 = 10$. An imbalance like this makes lottery scheduling feel very imbalanced and makes it seem like certain tasks are hogging the CPU. while these tasks may be hogging the CPU, thats because they were given the ticekts to do so.

### When running with two jobs of length 100 and equal ticket allocations of 100 (`-l 100:100,100:100`), how unfair is the scheduler? Run with some different random seeds to determine the (probabilistic) answer; let unfairness be determined by how much earlier one job finishes than the other.

Probabalistically, the scheduler is completely fair. The worst fairness value I found was (181/200)

### How does your answer to the previous question change as the quantum size (`-q`) gets larger?

As the quantum size gets larger, the schduler is still completely fair from a probabalistic standpoint, but it often appears less fair as it approaches completely fair with more scheudling decisions. 

### Can you make a version of the graph that is found in the chapter? What else would be worth exploring? How would the graph look with a stride scheduler?

I don't know what they mean by "make", but bascially assuming quantum stays the same we see the exact same phenomenon as the graph shows. If quantum grows that is the equivalent of the job length shrinking because the scheudler gets to make fewer decisions. 

Stride based scheduling is deterministically fair, so it would be fair regardless of the job length. Technically, it would be perfectly fair so long as the job length was multiple of the scheduling cycle. If it was not, it would become more fair as the job got longer becuase the last cycle being unfair would not impact the overall runtimes as much. 