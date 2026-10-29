### Compute the seek, rotation, and transfer times for the following sets of requests: `-a 0`, `-a 6`, `-a 30`, `-a 7,30,8`, and finally `-a 10,11,12,13`.

### Do the same requests above, but change the seek rate to different values: `-S 2`, `-S 4`, `-S 8`, `-S 10`, `-S 40`, `-S 0.1`. How do the times change?

Once we get past S = 4 we stop seeing benefits. It's not that we couldn't see any more benefits, but with the set of reads we are looking at, it just stops making a difference. There are no remaining cases where the sector passes us by while we are trying to seek the track. Setting the seek to 1 obviosly basically destroys the perfromance of anything that requires seeking tracks because it will take 400 time units to seek between tracks, which is longer than even a full rotation of the disk takes.

### Do the same requests above, but change the rotation rate: `-R 0.1`, `-R 0.5`, `-R 0.01`. How do the times change?

At .1 and .01, rotation time totally dominates the time of everything. One thing I forgot to think about at first was that a decrease in rotation speed will also increase data transfer times. At .5, we still miss sector 8 when seeking from sector 30, so something slightly slower on that could actually save us time.  

### FIFO is not always best, e.g., with the request stream `-a 7,30,8`, what order should the requests be processed in? Run the shortest seek-time first (SSTF) scheduler (`-p SSTF`) on this workload; how long should it take (seek, rotation, transfer) for each request to be served?

We should process them 7->8->30.The first request should take 45 time intervals (15 rot, 30 trans), the second should take 30 as it is sequential, and then last should take 300(80 seek, 190 rot, 30 trans).

### Now use the shortest access-time first (SATF) scheduler (`-p SATF`). Does it make any difference for `-a 7,30,8` workload? Find a set of requests where SATF outperforms SSTF; more generally, when is SATF better than SSTF?

It does not make a difference for those requests. A set of requests where SATF is more performant is 7,35,6. In this case, it makes the disk can read the sectors in that order and complete everything in just about 1 rotation. However, if it chooses to read the requests in the same track first, it will burn a whole rotation to read 6 after 7 and miss reading 35 in between. 

### Here is a request stream to try: `-a 10,11,12,13`. What goes poorly when it runs? Try adding track skew to address this problem (`-o skew`). Given the default seek rate, what should the skew be to maximize performance? What about for different seek rates (e.g., `-S 2`, `-S 4`)? In general, could you write a formula to figure out the skew?

Because there is no skew to the tracks by default, there is a full rotation necessary. Sector 11 is the end of the outermost track, so when you seek to the next track to read chapter 12, the time needed to seek causes you to miss the sector. The best skew we can assign given the constraints (only integer skews) is 2. However, if we could assign skew however we wanted, the best value would be 1.33... blocks or 40 time units worth of distance. As the seek time decreases, so does the desired skew. Generally skew (in blocks) = (time to seek 1 track) / time to read a block. Perhaps more usefully, skew as a degree would be (time to seek 1 track) / (360 / num sectors in a track), assuming all tracks had the same number of blocks (wrong). 

### Specify a disk with different density per zone, e.g., `-z 10,20,30`, which specifies the angular difference between blocks on the outer, middle, and inner tracks. Run some random requests (e.g., `-a -1` `-A 5,-1,0`, which specifies that random requests should be used via the `-a -1` flag and that five requests ranging from 0 to the max be generated), and compute the seek, rotation, and transfer times. Use different random seeds. What is the bandwidth (in sectors per unit time) on the outer, middle, and inner tracks?

The bandwith per each track given the above density is .1 sector / time unit, .05 sector / time unit, and .033 sector / time unit.

### A scheduling window determines how many requests the disk can examine at once. Generate random workloads (e.g., `-A 1000,-1,0`, with different seeds) and see how long the SATF scheduler takes when the scheduling window is changed from 1 up to the number of requests. How big of a window is needed to maximize performance? Hint: use the `-c` flag and don’t turn on graphics (-G) to run these quickly. When the scheduling window is set to 1, does it matter which policy you are using?

I tested at 1, 10, 100, 500, and 1000 and for my tests I stopped seeing improvements at 500. I think the exact number would depend on how the requests were distributed. If you have enough request to basically just move through the disk in order, than you won't see improvements after that. When the scheduling window is set to 1 it does not matter which policy you are using because you are essentially forced to handle the requests in order anyway. 

### Create a series of requests to starve a particular request, assuming an SATF policy. Given that sequence, how does it perform if you use a bounded SATF (BSATF) scheduling approach? In this approach, you specify the scheduling window (e.g., `-w 4`); the scheduler only moves onto the next window of requests when all requests in the current window have been serviced. Does this solve starvation? How does it perform, as compared to SATF? In general, how should a disk make this trade-off between performance and starvation avoidance?

To starve a request, we could make this order of requests: 7,24,9,11,1,3,5,7. If you kept jumping by 2 sectors in the outer ring the request for 24 would never be served. Using the BSTAF solved the problem but we took a pretty big hit to time (555 -> 1005). This will solve starvation because you can't just keep piling on request that will jump ahead of the request that was being starved. At worst, any request will be handled after w - 1 other requests. This will cause a performance hit for the average request but if your window was big I think the hit would be pretty small. Additionally, anything that would get starved for the duration of that window has probably been starved for a while and needs to be handled. 

### All the scheduling policies we have looked at thus far are greedy; they pick the next best option instead of looking for an optimal schedule. Can you find a set of requests in which greedy is not optimal?

Sure, using 7,33 from a starting position of 6. It is faster to serve 33 and then 7 but the algorithms we discussed would serve 7 first. 