### Run `./lfs.py -n 3`, perhaps varying the seed (`-s`). Can you figure out which commands were run to generate the final file system contents? Can you tell which order those commands were issued? Finally, can you determine the liveness of each block in the final file system state? Use `-o` to show which commands were run, and `-c` to show the liveness of the final file system state. How much harder does the task become for you as you increase the number of commands issued (i.e., change `-n 3` to `-n 5`)?

Sometimes I can figure out which order the commands were run in but not always; however, I can figure out the commands the liveliness of each block. This becomes harder as the number of commands increases. 

### If you find the above painful, you can help yourself a little bit by showing the set of updates caused by each specific command. To do so, run `./lfs.py -n 3 -i`. Now see if it is easier to understand what each command must have been. Change the random seed to get different commands to interpret (e.g., `-s 1`, `-s 2`, `-s 3`, etc.).

### To further test your ability to figure out what updates are made to disk by each command, run the following: `./lfs.py -o -F -s 100` (and perhaps a few other random seeds). This just shows a set of commands and does NOT show you the final state of the file system. Can you reason about what the final state of the file system must be?

### Now see if you can determine which files and directories are live after a number of file and directory operations. Run `tt ./lfs.py -n 20 -s 1` and then examine the final file system state. Can you figure out which pathnames are valid? Run `tt ./lfs.py -n 20 -s 1 -c -v` to see the results. Run with `-o` to see if your answers match up given the series of random commands. Use different random seeds to get more problems.

### Now let’s issue some specific commands. First, let’s create a file and write to it repeatedly. To do so, use the `-L` flag, which lets you specify specific commands to execute. Let’s create the file ”/foo” and write to it four times: `-L c,/foo:w,/foo,0,1:w,/foo,1,1:w,/foo,2,1:w,/foo,3,1 -o`. See if you can determine the liveness of the final file system state; use `-c` to check your answers.

### Now, let’s do the same thing, but with a single write operation instead of four. Run `./lfs.py -o -L c,/foo:w,/foo,0,4` to create file ”/foo” and write 4 blocks with a single write operation. Compute the liveness again, and check if you are right with `-c`. What is the main difference between writing a file all at once (as we do here) versus doing it one block at a time (as above)? What does this tell you about the importance of buffering updates in main memory as the real LFS does?

Writing the file all at once lets us write (and read) the data sequentially. In the previous example the data gets fragmented. Buffering in memory gives the file system the opportunity to write writes that could be written seqeuntially sequentially if they come in fast enough. 

### Let’s do another specific example. First, run the following: `./lfs.py -L c,/foo:w,/foo,0,1`. What does this set of commands do? Now, run `./lfs.py -L c,/foo:w,/foo,7,1`. What does this set of commands do? How are the two different? What can you tell about the size field in the inode from these two sets of commands?

They both write to files, but the later writes to the file with an offset of 7. You can tell because the second inode has a size of 8 despite having the same data written as the first command, which as a size of 1. 

### Now let’s look explicitly at file creation versus directory creation. Run simulations `./lfs.py -L c,/foo` and `./lfs.py -L d,/foo` to create a file and then a directory. What is similar about these runs, and what is different?

The new directory immediately requires a block to store data in whereas the file does not require this until it has been written to, so there is one less block of data writeen. They both require an update to the data in the root directory and both of their inodes get appended after the new data. 

### The LFS simulator supports hard links as well. Run the following to study how they work: `./lfs.py -L c,/foo:l,/foo,/bar:l,/foo,/goo -o -i`. What blocks are written out when a hard link is created? How is this similar to just creating a new file, and how is it different? How does the reference count field change as links are created?

It's similar in that a new entry has to be added to the directory data whether you are linking a file or creating a file for the first time. It's different in that linking a file does not create new inode entry. Linking a file just adds a new entry into the directory data and increments the reference count of the files inodes. 

### LFS makes many different policy decisions. We do not explore many of them here – perhaps something left for the future – but here is a simple one we do explore: the choice of inode number. First, run `./lfs.py -p c100 -n 10 -o -a s` to show the usual behavior with the ”sequential” allocation policy, which tries to use free inode numbers nearest to zero. Then, change to a ”random” policy by running `./lfs.py -p c100 -n 10 -o -a r` (the `-p c100` flag ensures 100 percent of the random operations are file creations). What on-disk differences does a random policy versus a sequential policy result in? What does this say about the importance of choosing inode numbers in a real LFS?

The default policy keeps the inodes in a tighter spread closer to 0. This should improve read performance by reducing the seek time. 

### One last thing we’ve been assuming is that the LFS simulator always updates the checkpoint region after each update. In the real LFS, that isn’t the case: it is updated periodically to avoid long seeks. Run `./lfs.py -N -i -o -s 1000` to see some operations and the intermediate and final states of the file system when the checkpoint region isn’t forced to disk. What would happen if the checkpoint region is never updated? What if it is updated periodically? Could you figure out how to recover the file system to the latest state by rolling forward in the log?

If it was never updated you could still figure out the correct state of the system with rolling forward but it would take much longer and only work if no data was cleared from the system. When updating periodically it doesn't seem that difficut to roll forward the system if you crashed, assuming it's every 30 seconds or so. 

