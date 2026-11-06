### Run the simulator with some different random seeds (say 17, 18, 19, 20), and see if you can figure out which operations must have taken place between each state change.

### Now do the same, using different random seeds (say 21, 22, 23, 24), except run with the `-r` flag, thus making you guess the state change while being shown the operation. What can you conclude about the inode and data-block allocation algorithms, in terms of which blocks they prefer to allocate?

They seem to always just allocate the first one that they find reading the address space from left to right.

### Now reduce the number of data blocks in the file system, to very low numbers (say two), and run the simulator for a hundred or so requests. What types of files end up in the file system in this highly constrained layout? What types of operations would fail?

I couldn't get this to run without it erroring. However, if we were consumed by teh amount of data blocks in the system I imagine the only files that could fill up the system were hard links and unwritten files, as they don't take up more data blocks. We could have a root directory full of files that were never written to and links to those files

### Now do the same, but with inodes. With very few inodes, what types of operations can succeed? Which will usually fail? What is the final state of the file system likely to be?

This also refused to run for me. However, if inodes were the limiting factor than we could repeatedly write to files or create links and unlink things. 