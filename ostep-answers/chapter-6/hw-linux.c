/*
HW part 1 done in mac file...

Measuring the cost of a context switch is a little trickier. The lmbench benchmark does so by 
running two processes on a single CPU, and setting up two UNIX pipes between them; a pipe is just 
one of many ways processes in a UNIX system can communicate with one another. The first process then 
issues a write to the first pipe, and waits for a read on the second; upon seeing the first process 
waiting for something to read from the second pipe, the OS puts the first process in the blocked state,
and switches to the other process, which reads from the first pipe and then writes to the second. When 
the second process tries to read from the first pipe again, it blocks, and thus the back-and-forth 
cycle of communication continues. By measuring the cost of communicating like this repeatedly,
lmbench can make a good estimate of the cost of a context switch. You can try to re-create something similar 
here, using pipes, or perhaps some other communication mechanism such as UNIX sockets.

One difficulty in measuring context-switch cost arises in systems with more than one CPU; what you need 
to do on such a system is ensure that your context-switching processes are located on the same processor. 
Fortunately, most operating systems have calls to bind a process to a particular processor; on Linux, 
for example, the sched setaffinity() call is what you’re looking for. By ensuring both processes are on 
the same processor, you are making sure to measure the cost of the OS stopping one process and restoring 
another on the same CPU.

Mac does not have any methods exposed to devs to pin a process to a given CPU. To measure this, I am instead 
creating a linux VM using UTM with 1 vCPU so that everything will run as though it's pinned to the same CPU.

I know this will add overhead from the hypervisor and it won't be exact but I don't want to boot linux or get 
access to another box so here we are. 
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000
#define ITERATIONS 1000000

int main (){
	int first_pipe[2];
	int second_pipe[2];
	pipe(first_pipe);
	pipe(second_pipe);

	char * child_message = "C";
	size_t child_message_len = sizeof("C");

	char * parent_message = "P";
	size_t parent_message_len = sizeof("P");

	char child_buffer[parent_message_len];
	char parent_buffer[child_message_len];

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	pid_t child = fork();

	if(child == 0){
		for (size_t i = 0; i < ITERATIONS; i++) {
			ssize_t bytes_written = write(second_pipe[1], child_message, child_message_len);

			if (bytes_written == -1){
				printf("Child Write failed!");
				exit(1);
			}

			ssize_t bytes_read = read(first_pipe[0], child_buffer, parent_message_len);

			if (bytes_read == -1){
				printf("Child Read Failed!");
				exit(1);
			}
		}
	}

	else {
		for (size_t j = 0; j < ITERATIONS; j++){
			ssize_t bytes_written = write(first_pipe[1], parent_message, parent_message_len);

			if (bytes_written == -1){
				printf("Parent Write failed!");
				exit(1);
			}

			ssize_t bytes_read = read(second_pipe[0], parent_buffer, child_message_len);

			if (bytes_read == -1){
				printf("Parent Read Failed!");
				exit(1);
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &end);

		double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
		double time_per_context_switch = time_spent / ITERATIONS;

        // Always seeing 0.0000020000 or 0.0000010000 or 1-2 microseconds 
		printf("Estimated time per context swtich: %.10f\n", time_per_context_switch);
	}

}
