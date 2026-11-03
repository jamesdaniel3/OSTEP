### Use the simulator to perform some basic RAID mapping tests. Run with different levels (0, 1, 4, 5) and see if you can figure out the mappings of a set of requests. For RAID-5, see if you can figure out the difference between left-symmetric and left-asymmetric layouts. Use some different random seeds to generate different problems than above.

The naming convention of these confuse me, but it seems like left-asymmetric alignment always starts counting at the leftmost available alignment. So it would look like:

[0, 1, 2, 3, P]
[4, 5, 6, P, 7]
[8, 9, P, 10, 11]
[12, P, 13, 14, 15]
[P, 16, 17, 18, 19]

On the other side of this, left-symmetric alignment starts the block in the next offset directly below the parition block for the current offset, so counts can wrap around in this scheme.

[0, 1, 2, 3, P]
[5, 6, 7, P, 4]
[10, 11, P, 8, 9]
[15, P, 12, 13, 14]
[P, 16, 17, 18 ,19]

### Do the same as the first problem, but this time vary the chunk size with `-C`. How does chunk size change the mappings?

The larger chunk size means that we no longer see a new block be written to the next disk every time. Instead we could see 2 or more blocks written in seqeuntial memory to a block. 

Ex. 

[0, 2, 4, 6, P]
[1, 3, 5, 7, P]
[8, 10, 12, P, 14]
[9, 11, 13, P, 15]

A small chunk size means that files are more likely to be striped and can therefore be read more quickly in parallel. However, the positioning time increases because there is more positioning to be done and the positioning time is the max of all of the disks that have to be positioned on. Big chunks benefit less from parallelism but can improve reads and writes by handling concurrent smaller requests. The best chunk size depends on the workload the user has. 

### Do the same as above, but use the `-r` flag to reverse the nature of each problem.

### Now use the reverse flag but increase the size of each request with the `-S` flag. Try specifying sizes of 8k, 12k, and 16k, while varying the RAID level. What happens to the underlying I/O pattern when the size of the request increases? Make sure to try this with the sequential workload too (`-W sequential`); for what request sizes are RAID-4 and RAID-5 much more I/O efficient?

Raid 4 and 5 are much more efficient when you are able to write the full strupe at a time, so when the request is of size (num disks * chunk size). This is esepcially true when the work is sequential. 

### Use the timing mode of the simulator (`-t`) to estimate the performance of 100 random reads to the RAID, while varying the RAID levels, using 4 disks.

Striping is by far the fastest, mirroring is nearly twice as slow, than raid 4 and 5 begin to get back some of the performance gains. Everything runs pretty much as expected relative to each other. 

### Do the same as above, but increase the number of disks. How does the performance of each RAID level scale as the number of disks increases?

The marginal improvements in speed decrease every time I double the number of disks, so we actually end up with even mirroring appraoching the speed of striping with enough disks. Its all relative and I expect if I let the range of requestable address grow largely than mirroring would still be the fastest. However, the impact of using a disk for parity is amoritized as we have more disks so raid 4 and 5 can approach the perfomance of striping when reading. 

### Do the same as above, but use all writes (`-w 100`) instead of reads. How does the performance of each RAID level scale now? Can you do a rough estimate of the time it will take to complete the workload of 100 random writes?

Striping is pretty much the same because it has no redundancy. Mirroring and raid 4 and 5 both start out struggling way more than they did with reads because they ahve to write everything in at least two registers. Raid 1 is more performant because it does not have to manage the XOR operation. With enough disks things approach similar performance again but Raid 1, 4, 5 take more disks to approach the behavior of striping than they did for reads.

### Run the timing mode one last time, but this time with a sequential workload (`-W sequential`). How does the performance vary with RAID level, and when doing reads versus writes? How about when varying the size of each request? What size should you write to a RAID when using RAID-4 or RAID-5?

Al of these are immediately more performant with the sequential work loads. When using a raid 4 or 5 array, you would get the most benefit out of writing data that is equal to num disks * chunk size at a time. This would allow you to have all the info you ned to make the fifth write so you wouldn't have to do as much extra work to update the parity column. It essentailly lets you do the most work assuming the cost of updating the parity is similar. 