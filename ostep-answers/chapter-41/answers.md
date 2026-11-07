### Examine the file `in.largefile`, and then run the simulator with flag `-f in.largefile` and `-L 4`. The latter sets the large-file exception to 4 blocks. What will the resulting allocation look like? Run with `-c` to check.

I think there will be 4 blocks of data in each of the 10 blocks, starting with the file in the first block b/c it's created at the root. 

### Now run with `-L 30`. What do you expect to see? Once again, turn on `-c` to see if you were right. You can also use `-S` to see exactly which blocks were allocated to the file `/a`.

The first block group will get 30 blocks allocated for the file and the next will get 10.

Looks like I was slighlty wrong about this. There are only 30 blocks per group and the first block group already has one blaock allocated for the root firectory, so 29 blocks get added to the first block group and the remaining 11 go to the next.

### Now we will compute some statistics about the file. The first is something we call filespan, which is the max distance between any two data blocks of the file or between the inode and any data block. Calculate the filespan of /a. Run `ffs.py -f in.largefile -L 4 -T -c` to see what it is. Do the same with `-L 100`. What difference do you expect in filespan as the large-file exception parameter changes from low values to high values?

As the large file exception becomes larger, the filespan should decrease because the file will be split across fewer block groups. 

### Now let’s look at a new input file, `in.manyfiles`. How do you think the FFS policy will lay these files out across groups? (you can run with `-v` to see what files and directories are created, or just cat `in.manyfiles`). Run the simulator with `-c` to see if you were right.

### A metric to evaluate FFS is called dirspan. This metric calculates the spread of files within a particular directory, specifically the max distance between the inodes and data blocks of all files in the directory and the inode and data block of the directory itself. Run with `in.manyfiles` and the `-T` flag, and calculate the dirspan of the three directories. Run with `-c` to check. How good of a job does FFS do in minimizing dirspan?

It seems to do a good job to me of minimizing dirpsan. Given the input we have I don't think we could have had a better dirspan. 

### Now change the size of the inode table per group to 5 (`-i 5`). How do you think this will change the layout of the files? Run with `-c` to see if you were right. How does it affect the dirspan?

Previously, all of the directories had more than 4 files. This will force them to split their files across block groups and increase dirspan.

### Which group should FFS place inode of a new directory in? The default (simulator) policy looks for the group with the most free inodes. A different policy looks for a set of groups with the most free inodes. For example, if you run with `-A 2`, when allocating a new directory, the simulator will look at groups in pairs and pick the best pair for the allocation. Run `./ffs.py -f in.manyfiles -i 5 -A 2 -c` to see how allocation changes with this strategy. How does it affect dirspan? Why might this policy be good?

This helps the dirspan because it basicallly accounts for the spillover. We start directories with enough room to spillover and protect previous directories from getting interrupted. This way files that need to go to the next block group are more likely to be able to go to the next one rather than havign to skip one or more because something has already been put there.

### One last policy change we will explore relates to file fragmentation. Run `./ffs.py -f in.fragmented -v` and see if you can predict how the files that remain are allocated. Run with `-c` to confirm your answer. What is interesting about the data layout of file /i? Why is it problematic?

File i is filling all the gaps that were left by files that were deleted. This means that we can't read it continuously because it is spit up. 

### A new policy, which we call contiguous allocation (`-C`), tries to ensure that each file is allocated contiguously. Specifically, with `-C n`, the file system tries to ensure that n contiguous blocks are free within a group before allocating a block. Run `./ffs.py -f in.fragmented -v -C 2 -c` to see the difference. How does layout change as the parameter passed to `-C` increases? Finally, how does `-C` affect filespan and dirspan?

Increasing C increases dirspan and filespan because it basically spreads files out within their block, pushing them further from the inodes. In this given files, `-C 2` changes the layout and anything about it does not, but that's only because there are no gaps that are > 1 block long. With this settings, we basically just can't fill in spaces that are < N blocks long. 