/*
In this homework, you’ll measure the costs of a system call and context
switch. Measuring the cost of a system call is relatively easy. For example,
you could repeatedly call a simple system call (e.g., performing a 0-byte
read), and time how long it takes; dividing the time by the number of
iterations gives you an estimate of the cost of a system call.

One thing you’ll have to take into account is the precision and accuracy of your timer. 
A typical timer that you can use is gettimeofday(); read the man page for details. 
What you’ll see there is that gettimeofday() returns the time in microseconds since 1970; 
however, this does not mean that the timer is precise to the microsecond. Measure back-to-back 
calls to gettimeofday() to learn something about how precise the timer really is; this will 
tell you how many iterations of your null system-call test you’ll have to run in order to get 
a good measurement result. If gettimeofday() is not precise enough for you, you might look into
using the rdtsc instruction available on x86 machines.

CONTEXT SWITCH MEASUREMENT DONE IN LINUX FILE...
*/

#include <time.h>
#include <unistd.h>
#include <stdio.h>

// How do we make sure this does not optimize away the loop when comipiling? 

#define BILLION 1000000000
#define NUM_ITERATIONS 10000000
int main(){
    struct timespec start, end; 

    clock_gettime(CLOCK_MONOTONIC, &start);
    // execute an instruction a bunch of times 

    for (int i = 0; i < NUM_ITERATIONS; i ++) {
        read(STDIN_FILENO, NULL, 0);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    double time_per_system_call = time_spent / NUM_ITERATIONS;

    // Always seeing 0.0000002000 or 0.0000003000 ~ 200-300 nanoseconds or .2-.3 microseconds 
    printf("Estimated time per system call: %.10f\n", time_per_system_call);
}