#### With a linear page table, you need a single register to locate the page table, assuming that hardware does the lookup upon a TLB miss. How many registers do you need to locate a two-level page table? A three-level table?

I think you still only need one register no matter what. You have a register that points you to the top level of the table, and then the entries in the table enable you to move calculate physical memory addresses from there on.

### Use the simulator to perform translations given random seeds 0, 1, and 2, and check your answers using the `-c` flag. How many memory references are needed to perform each lookup?

Each lookup requires 3 memory references if it does not fail sooner. 
- Read the page directory entry using the first 5 bits.
- Read the page table entry using the next 5 bits.
- Read the actual data using the PFN from the page table entry and the final 5-bit offset.

#### Given your understanding of how cache memory works, how do you think memory references to the page table will behave in the cache? Will they lead to lots of cache hits (and thus fast accesses?) Or lots of misses (and thus slow accesses)?

I think the page table approach will lead to more cache hits. All requests will hit the top level page table and from there we can probably expect many requests to hit the same seconday page table due to locality of memory and cache it. 