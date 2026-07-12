### Before doing any translations, let’s use the simulator to study how linear page tables change size given different parameters. Compute the size of linear page tables as different parameters change. Some suggested inputs are below; by using the `-v` flag, you can see how many page-table entries are filled. First, to understand how linear page table size changes as the address space grows, run with these flags: `-P 1k -a 1m -p 512m -v -n 0`, `-P 1k -a 2m -p 512m -v -n 0`, `-P 1k -a 4m -p 512m -v -n 0`.

### Then, let’s understand how linear page table size changes as page size grows. Before running any of these, try to think about the expected trends. How should page-table size change as the address space grows? As the page size grows? Why not use big pages in general? `-P 1k -a 1m -p 512m -v -n 0`, `-P 2k -a 1m -p 512m -v -n 0`,  `-P 4k -a 1m -p 512m -v -n 0`.

As the address space grows, the page table would grow in size. As the page size grows, the page table would shrink in size. I think it's probably bad to use big pages because we would end up with a lot of internal fragmentation or dead space within the page. 

### Now let’s do some translations. Start with some small examples, and change the number of pages that are allocated to the address space with the `-u` flag. What happens as you increase the percentage of pages that are allocated in each address space? `-P 1k -a 16k -p 32k -v -u 0`, `-P 1k -a 16k -p 32k -v -u 25`, `-P 1k -a 16k -p 32k -v -u 50`, `-P 1k -a 16k -p 32k -v -u 75`, `-P 1k -a 16k -p 32k -v -u 100`

As the percentage of pages that are allocated increases, more virtual address are valid. 

### Now let’s try some different random seeds, and some different (and sometimes quite crazy) address-space parameters, for variety. Which of these parameter combinations are unrealistic? Why? `-P 8 -a 32 -p 1024 -v -s 1`, `-P 8k -a 32k -p 1m -v -s 2`, `-P 1m -a 256m -p 512m -v -s 3`

I guess I would say the first and third set of arguments are unrealistic because of the size of the pages. 


### Use the program to try out some other problems. Can you find the limits of where the program doesn’t work anymore? For example, what happens if the address-space size is bigger than physical memory?

If the addres space is bigger than the physical memory than the program quits. The program also quits if the page size is > the address space or the page size is not a multiple of the address space. 