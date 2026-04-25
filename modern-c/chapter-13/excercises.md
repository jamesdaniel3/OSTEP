## Excercise 1

#### Compute the space needed to allocate a vector of doubles where there is 1 value for every living person and determine if allocating such a vector is feasible on your platform

On my system, a double is 8 bytes. There are roughly 8.3 billion living people. 

This array would require: $8 * 8.3B = 66.4B$ bytes or $66.4$ gigabytes. 

This is not feasible. 


## Excercise 2

#### Write implementations of the missing functions

They gave me all of the functions, aside for the `circular_append`, `circular_pop`, and `circular_resize`. The book says to not write the `circular_resize` function in the text, so I have skipped that one for the moment. I adjust the guard conditions to make the code easier to read and adjusted the line in `circular_element` that I commented on. Code can be found in circular_buffer_excercises.

## Excercise 3. 

#### Implement shrinking of the table; it is important to reorganize the table contents before calling realloc

Code and tests can be found in circular_buffer_excercises folder. I read this question and then spent spare time messing with it for a couple days and forgot that it mentioned using realloc. I guess it wanted me to shift and then realloc the array; I went with just freeing the array and creating a new one in it's place. I am choosing to leave my implementation in. 