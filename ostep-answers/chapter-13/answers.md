### The first Linux tool you should check out is the very simple tool `free`. First, type man `free` and read its entire manual page; it’s short, don’t worry!

### Now, run `free`, perhaps using some of the arguments that might be useful (e.g., `-m`, to display memory totals in megabytes). How much memory is in your system? How much is free? Do these numbers match your intuition?

Free says I have 1.6 GB of memory and 1 GB free. These numbers roughly make sense as I am running a VM that I only gave 2 gigs of memory. However, I am not sure what is using up .6 gigs. 

### Next, create a little program that uses a certain amount of memory, called `memory-user.c`. This program should take one commandline argument: the number of megabytes of memory it will use. When run, it should allocate an array, and constantly stream through the array, touching each entry. The program should do this indefinitely, or, perhaps, for a certain amount of time also specified at the command line.

### Now, while running your memory-user program, also (in a different terminal window, but on the same machine) run the `free` tool. How do the memory usage totals change when your program is running? How about when you kill the memory-user program? Do the numbers match your expectations? Try this for different amounts of memory usage. What happens when you use really large amounts of memory?

I can watch the used and free memory move up and down based on how much memory I allow the toy program to use up. 

### Let’s try one more tool, known as `pmap`. Spend some time, and read the `pmap` manual page in detail.

### To use `pmap`, you have to know the process ID of the process you’re interested in. Thus, first run `ps auxw` to see a list of all processes; then, pick an interesting one, such as a browser. You can also use your memory-user program in this case (indeed, you can even have that program call `getpid()` and print out its PID for your convenience).

### Now run `pmap` on some of these processes, using various flags (like `-X`) to reveal many details about the process. What do you see? How many different entities make up a modern address space, as opposed to our simple conception of code/stack/heap?

I hardly know what I am reading but the table I see when I run with `-X`  on the `memory-user` program has 18 rows.  

### Finally, let’s run `pmap` on your memory-user program, with different amounts of used memory. What do you see here? Does the output from pmap match your expectations?

The output generally matches my expectations in that it seems like the total memory of the program is dominated by the size of the allocated memory and it pretty much grows in step with that number. The outputs are a little strange to me because they are always slightly less than the amount of memory I requested be allocated when I feel like they should be slightly more. 