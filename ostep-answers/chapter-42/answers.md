### First, run `fsck.py -D`; this flag turns off any corruption, and thus you can use it to generate a random file system, and see if you can determine which files and directories are in there. So, go ahead and do that! Use the `-p` flag to see if you were right. Try this for a few different randomly-generated file systems by setting the seed (`-s`) to different values, like 1, 2, and 3.

### Now, let’s introduce a corruption. Run `fsck.py -S 1` to start. Can you see what inconsistency is introduced? How would you fix it in a real file system repair tool? Use `-c` to check if you were right.

There are 1 more inodes in the inodes array than there are allocated in the inode bitmap. Specifically, the one at position 13. This inode claims to have two references and is referenced twice in the data; I would probably restore the system by just updating the bitmap. 

### Change the seed to `-S 3` or `-S 19`; which inconsistency do you see? Use `-c` to check your answer. What is different in these two cases?

In the first example, there is a file that claims to have more references than are shown in the data. In the secone emaple, there isa  directory that says it has one reference but actually has two in the data. 

### Change the seed to `-S 5`; which inconsistency do you see? How hard would it be to fix this problem in an automatic way? Use `-c` to check your answer. Then, introduce a similar inconsistency with `-S 38`; is this harder/possible to detect? Finally, use `-S 642`; is this inconsistency detectable? If so, how would you fix the file system?

I take issue with the first example. It says that a directory was currupted because a file in the directory had it's name changed. This is unprovable anywhere in teh info they have given us. I have no way of knowing if a different hard link was created or anything you can only flag it by saying that's different from last run. I don't think it could be confirmed as data curruption given one run. The second example has the name of hte .. entry wrong in one of it's directories. Instead of listing .. it lists b. The second one is way easier to detect. The last one is definetly detectable. There are two directory enties at the root with the same name, which is not allowed. I don't think there is a way to recover it perfectly as I don't know how you would know the original names of the directories. I think they would be stored in the inodes but with the info we have they aren't. Assuming they aren't I would just pick a new name for one. If they are I would check both and update appropriately. 

### Change the seed to `-S 6` or `-S 13`; which inconsistency do you see? Use `-c` to check your answer. What is the difference across these two cases? What should the repair tool do when encountering such a situation?

Both of these have 1 more inode allocated in the inode array than is shown in the bitmap. Neither of these inodes have any references in the data section despite both saying they have one, so it seems that the cleanup to the inode was interrupted. In the first case, it is a file that is missing. In the second, it is a directory. For both cases I think the best option is to just remove the inode entry becasue there are no references to it in the data. 

### Change the seed to `-S 9`; which inconsistency do you see? Use `-c` to check your answer. Which piece of information should a checkand-repair tool trust in this case?

There is an entry in the inode table that says it is a directory, but the corresponding entries referencing it in the data table treat it like a file. I would trust the data table for a few reasons. First, there can be no hard links to directories and the data table shows a hard link. Second, the data table does not contain . or .. references to this directory. 

### Change the seed to `-S 15`; which inconsistency do you see? Use `-c` to check your answer. What can a repair tool do in this case? If no repair is possible, how much data is lost?

This is the opposite problem, the first inode says it is a file when it is actually a directory based on the data. I think the repair tool could just change the type of the inode because the first inode is always a directory. If I'm wrong about that and no repair is possible, I think you might lose all data to access anything you would have to read it's path through the root and that path would always start with a file, basically cutting off your access to all other files. 

### Change the seed to `-S 10`; which inconsistency do you see? Use `-c` to check your answer. Is there redundancy in the file system structure here that can help a repair?

In this one, the .. entry of one of the directories is pointing to the wrong inode entry. This can be fixed by the redundancy in teh file system because you can traverse the file tree and when you are in the parent directory you will see it's children. So going to this child you can know it's parent's inode and know that the chile has it wrong. 

### Change the seed to `-S 16` and `-S 20`; which inconsistency do you see? Use `-c` to check your answer. How should the repair tool fix the problem?

The first example shows a file as being allocated when the data table doesn't have anything in it at the referenced block. The second example shows a directory pointing at the wrong block. There is data allocated that it is supposed to use but the inode has the location wrong. I think the repair tool could fix the directory by updating it to the correct pointer as it could figure it out based on reading the data blocks. I don't think you can actually do anything about the first case because you dn't know that the empty block is not the user's data. 