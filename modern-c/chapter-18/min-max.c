/*
        Challenge 20

Our MAX macro might leave room for optimization in the case where the types of X and Y have different signedness. Implement an improved version that uses your implemented
traits to make an appropriate case disntinction that has a minimal number of comparisons for respective cases. Here, the property that your traits are ICE is essential for 
optimization because then compilers will be able to elimiate branches that are unreachable for given arguments.

Providing the same funtionality for the minimum of two integer values X and Y is even more delicate because the mathematical result of such an operation does not always fit 
into the same type as the maximum operation. First, convince youself that even if the types of X and Y are distinct, one of the two types is able to hold the result of the 
minimum operation. In, fact, even if both types are unsigned, it is sufficient to use the narrower type of them as the result type of the operation. Write a macro minunsigned 
that returns an ICE of value zero of the narrower unsigned type if presented with two unsigned expressions or type names. Then use this to implement a macro minreturn that 
returns an ICE of value zero that has a type that is able to hold the minimum value of its two arguments. Finally, use that for the implementation of the a mminimum operation MIN 
that returns the mathematically correct value for all pairwise combinations of integer types.  
*/

/*
    I'm gonna have to come back to most of this chapter. This is continuing to build on previous macros that built on even earlier macros that were unfinished. 
    I am hating this topic today but I want to keep making progress in the book so I figured I would write up excercise and maybe I will come back to it later

*/