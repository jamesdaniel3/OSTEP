### Generate random addresses with the following arguments: `-s 0 -n 10`, `-s 1 -n 10`, and `-s 2 -n 10`. Change the policy from FIFO, to LRU, to OPT. Compute whether each access in said address traces are hits or misses.

### For a cache of size 5, generate worst-case address reference streams for each of the following policies: FIFO, LRU, and MRU (worst-case reference streams cause the most misses possible). For the worst case reference streams, how much bigger of a cache is needed to improve performance dramatically and approach OPT?

I'm assuming the stream is of size 10. I guess I'm going to try to do this in a way such that OPT performs better than the policy in question. If not, we could just say the stream: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 would have a zero percent hit rate. 

For FIFO and LRU, the stream 0, 1, 2, 3, 4, 5, 0, 1, 2, 3 would have a 0% hit rate. However, if the cache was size 6, the OPT and FIFO would both have a hit rate of 100%. 

For MRU, the steram 0, 1, 2, 3, 4, 5, 4, 5, 4, 5 would result in a 0% hit rate. Again, increaseing the a cache size of 6 would bring performance equal with optimal. 

### Generate a random trace (i.e., use python and write a script that outputs random addresses, which you can then feed into the simulator). How would you expect the different policies to perform on such a trace?

I would expect OPT to do the best and all other policies to perform the same over the long term. We can't rely on locality so there is no reason that LRU would perform better or worse than FIFO or MRU. 

### Now generate a trace with some locality. How can you generate such a trace? How does LRU perform on it? How much better than RAND is LRU? How does CLOCK do? How about CLOCK with different numbers of clock bits?

I will generate a trace using the 80/20 rule descirbed in the book. To do this, I will make 80% of addresses generated come from 20% of the pages and 20% of addresses generated come from the remaining 80% of the pages. 

### Use a program like valgrind to instrument a real application and generate a virtual page reference stream. For example, running `valgrind --tool=lackey --trace-mem=yes ls` will output a nearly-complete reference trace of every instruction and data reference made by the program `ls`. To make this useful for the simulator above, you’ll have to first transform each virtual memory reference into a virtual page-number reference (done by masking off the offset and shifting the resulting bits downward). How big of a cache is needed for your application trace in order to satisfy a large fraction of requests? Plot a graph of its working set as the size of the cache increases.