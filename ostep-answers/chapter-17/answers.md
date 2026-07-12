### First run with the flags `-n 10 -H 0 -p BEST -s 0` to generate a few random allocations and frees. Can you predict what `alloc()`/`free()` will return? Can you guess the state of the free list after each request? What do you notice about the free list over time?

The free list grows a lot and because there is no coalesceing it just becomes a list of a bunch of small entries.

### How are the results different when using a WORST fit policy to search the free list (`-p WORST`)? What changes?

Using WORST, the freelist gets even longer because bascially every request takes a tiny slice off of the original big slice of memory. 

### What about when using FIRST fit (`-p FIRST`)? What speeds up when you use first fit?

Allocation should speed up when using FIRST fit. 

### For the above questions, how the list is kept ordered can affect the time it takes to find a free location for some of the policies. Use the different free list orderings (`-l ADDRSORT, -l SIZESORT+, -l SIZESORT-`) to see how the policies and the list orderings interact.

SIZESORT+ (increasing), pairs well with best fit because the first entry you find that is big enough is the best fit. It pairs poorly with worst fit because you have to traverse to the end of the list for every allocation. The opposite is true with SIZESORT-. Finding first fit probably works best with SIZESORT- because if the item at the start of the list fits you take it and if it doesn't you have a problem. Adderss sort is probably worse for lookup for all these policies but it would be much more helpful for coalescing. 

### Coalescing of a free list can be quite important. Increase the number of random allocations (say to `-n 1000`). What happens to larger allocation requests over time? Run with and without coalescing (i.e., without and with the `-C` flag). What differences in outcome do you see? How big is the free list over time in each case? Does the ordering of the list matter in this case?

Overtime, larger allocation requests fail without coalescing as there are no slots on the free list large enough to accomodate them. Without coalescing the freelist is usually finishing with a size in the 30s or 40s and with coalescing I was often seeing it finish with a size less than 5. 

The impact of the ordering of the list varies based on other parameters and based on what we are mesauring for. For example, if the policy is to choose the best fit from the free list, the ordering of the list does not matter from a fragmentation persepctive but you would want hte list to be ordered by size rather than address to allow for faster retrieval. If the list is not ordered by address, coalescing will be slower but it will still be possible. Ordering the list by size decreasing and allwoing the scheduler to pick first results in a lot of fragmentation if there is no coalescing. 

### What happens when you change the percent allocated fraction `-P` to higher than 50? What happens to allocations as it nears 100? What about as the percent nears 0?

This just results in us running out of space faster and eventually being unable to alloc more space, regardless of how much coalescing has occured. As we approach 0 it becomes easer to fufill requests. 

### What kind of specific requests can you make to generate a highly fragmented free space? Use the `-A` flag to create fragmented free lists, and see how different policies and options change the organization of the free list.

Like I mentioned earlier, the combination of `-p FIRST -l SIZESORT` without coalescing creates a very fragmented free list. 