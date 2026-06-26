### Run `./fork.py -s 10` and see which actions are taken. Can you predict what the process tree looks like at each step? Use the `-c` flag to check your answers. Try some different random seeds (`-s`) or add more actions (`-a`) to get the hang of it.

### One control the simulator gives you is the `fork_percentage`, controlled by the `-f` flag. The higher it is, the more likely the next action is a fork; the lower it is, the more likely the action is an exit. Run the simulator with a large number of actions (e.g., -a 100) and vary the fork_percentage from 0.1 to 0.9. What do you think the resulting final process trees will look like as the percentage changes? Check your answer with `-c`.

I think the trees will be much smaller with lower fork values than higher. Confirmed. The trees have much more nesting when there are more forks 

### Now, switch the output by using the `-t` flag (e.g., run `./fork.py -t`). Given a set of process trees, can you tell which actions were taken?

### One interesting thing to note is what happens when a child exits; what happens to its children in the process tree? To study this, let’s create a specific example: `./fork.py -A a+b,b+c,c+d,c+e,c-`. This example has process `a` create `b`, which in turn creates `c`, which then creates `d` and `e`. However, then, `c` exits. What do you think the process tree should like after the exit? What if you use the `-R` flag? Learn more about what happens to orphaned processes on your own to add more context.

After the exit, I think the process true would not include the orphaned processes of c. I think they would keep running but it wouldn't make sense for the tree to show them as it no longer has a connection to them. 

```
Final Process Tree:
        a
        ├── b
        ├── d
        └── e
```

It seems I was wrong, I think this allows the root to adopt the children when `c`orphans them. 

When I use the `-R` flag, it looks as though `b` adopts the children rather than `a`. 

```
Final Process Tree:
        a
        └── b
            ├── d
            └── e
```

After [a bit of googling](https://unix.stackexchange.com/questions/149319/new-parent-process-when-the-parent-process-dies) it seems that this behavior is implementation defined. I think the default behavior is to send orphaned processes to be children of process 1. However, many implementations allow nodes deeper in the tree to designate themselves as kind of a stopping point where any children orphaned in their subtree will become their children.  

### One last flag to explore is the `-F` flag, which skips intermediate steps and only asks to fill in the final process tree. Run `./fork.py -F` and see if you can write down the final tree by looking at the series of actions generated. Use different random seeds to try this a few times.

### Finally, use both `-t` and `-F` together. This shows the final process tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place? In which cases can you tell? In which can’t you tell? Try some different random seeds to delve into this question.

You can only tell if the tree requires exactly the number of actions shown to reach it's final state. Here is an example with some ambiguity:

```
Action?
Action?
Action?
Action?
Action?

        Final Process Tree:
                a
                └── c
```

Assuming process spawn in order, we know:
- a forks b
- b EXITS
- a forks c

We know that the other two actions must be that a process is forked and exits, but we don't know if the process is forked from a, c, or b. 

Here is a tree with no ambiguity:

```
Action?
Action?
Action?
Action?
Action?

            Final Process Tree:
                    a
                    └── b
                        └── c
                            └── d
                                ├── e
                                └── f
```

It must be that:
- a forks b
- b forks c
- c forks d
- d forks e
- d forks f