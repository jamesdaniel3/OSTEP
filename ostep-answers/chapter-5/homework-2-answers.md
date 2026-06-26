### Write a program that calls `fork()`. Before calling `fork()`, have the main process access a variable (e.g., `x`) and set its value to something (e.g., 100). What value is the variable in the child process? What happens to the variable when both the child and parent change the value of x?

```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int x = 100;
    int rc = fork();

    if (rc < 0){
        printf("Fork Failed!\n");
        exit(1);
    }

    else if (rc == 0) {
        // child process
        printf("Child sees: %d\n", x);
        x = 200;
        printf("After update child sees: %d\n", x);
    }

    else {
        printf("Parent sees: %d\n", x);
    }
}
```

I believe the parent and the child each get their own copy of all the variables in the program, starting at the value assigned to them when `fork()` was called. If the child updates the variable it will not update the parents copy and vice versa. 

### Write a program that opens a file (with the `open()` system call) and then calls `fork()` to create a new process. Can both the child and parent access the file descriptor returned by `open()`? What happens when they are writing to the file concurrently, i.e., at the same time?

From the man page for `fork`:

>The child process has its own copy of the parent's descriptors.  These descriptors reference the same underlying objects, so that, for instance, file pointers in file objects are shared between the child and the parent, so that an lseek(2) on a descriptor in the child process can affect a subsequent read or write by the parent.  This descriptor copying is also used by the shell to establish standard input and output for newly created processes as well as to set up pipes.

I'm not totally sure what happens if they write at the same time. I wonder if the scheduler would block one of the processes from writing to the descriptor while the other is writing. In my tests I have not been able to get them to overlap, I get all the text from one then all the text from the other, despite the processes running synchronously.

Okay I managed to get it to work. Apprently a file descriptor and a pointer to a `FILE` var are not the same thing. The `FILE` type in c is basically a wrapper around true file descriptors and it is importantly buffered. When I use true file descriptors and therefore do not have buffering I was able to get the processes to interleave their output:

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(){
    int fd = open("test.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd < 0) {
        perror("Error opening file\n");
        exit(1);
    }

    int rc = fork();

    if (rc < 0){
        printf("Fork Failed!\n");
        exit(1);
    }

    else if (rc == 0) {
        char * text = "Child\n";
        size_t text_size = sizeof("Child\n");
        for(int i = 1; i < 1000; i++) {
            ssize_t bytes_written = write(fd, text, text_size);
            if (bytes_written < 0) {
                printf("Write Failed!\n");
                exit(1);
            }
        }
    }

    else {
        char * text = "Parent\n";
        size_t text_size = sizeof("Parent\n");
        for(int i = 1; i < 1000; i++) {
            ssize_t bytes_written = write(fd, text, text_size);
            if (bytes_written < 0) {
                printf("Write Failed!\n");
                exit(1);
            }
        }
    }
}
```

The resulting output still looks horrible, the editor does not recognize it as a text file. I also wasn't able to get a true collision where you maybe saw half of one word and half of the other. I'm not sure if this is possible. 

### Write another program using `fork()`. The child process should print “hello”; the parent process should print “goodbye”. You should try to ensure that the child process always prints first; can you do this without calling `wait()` in the parent?

I'm assuming that they don't mean for us to achieve this by calling `waitpid()`. I bet there is a clear trick that they wanted me to do for this but this is the solution that came to mind for me:

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(){
    int fd = open("test.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd < 0) {
        perror("Error opening file\n");
        exit(1);
    }

    int rc = fork();

    if (rc < 0){
        printf("Fork Failed!\n");
        exit(1);
    }

    else if (rc == 0) {
        char * text = "Child\n";
        size_t text_size = sizeof("Child\n");
        ssize_t bytes_written = write(fd, text, text_size);

        if (bytes_written < 0) {
            printf("Write Failed!\n");
            exit(1);
        }

        printf("Hello\n");
    }

    else {
        while(lseek(fd, 0, SEEK_CUR) == 0) {
            continue;
        }
        printf("Goodbye\n");
    }
}
```

### Write a program that calls `fork()` and then calls some form of `exec()` to run the program /bin/ls. See if you can try all of the variants of `exec()`, including (on Linux) `execl()`, `execle()`, `execlp()`, `execv()`, `execvp()`, and `execvpe()`. Why do you think there are so many variants of the same basic call?

I managed to get them all to work but I'm not pasting all of those code snippets. I am not totally sure why there are so many different variants of the call. It seems to me that if you were to design this from scratch you could handle a lot of the variants with arguments rather than seperate functions. I saw one answer that I liked:

>You can't change the functionality of the old function names without breaking older programs that bind to the same library, so the different variants of the function are created with different parameters to handle different ways to execute.

This might be total nonsense but I imagine it is at least part of the answer. You also wouldn't want 6 extra parameters in the function or whatever. 

### Now write a program that uses `wait()` to wait for the child process to finish in the parent. What does `wait()` return? What happens if you use `wait()` in the child?

From man page: 

>If wait() returns due to a stopped or terminated child process, the process ID of the child is returned to the calling process.  Otherwise, a value of -1 is returned and errno is set to indicate the error.

Also from the man page: 

```
The wait() system call will fail and return immediately if:

[ECHILD]           The calling process has no existing unwaited-for child processes.
```

This is what happens when you use `wait()` in the child. 

### Write a slight modification of the previous program, this time using `waitpid()` instead of `wait()`. When would `waitpid()` be useful?

`waitpid()` would be useful if we had multiple child processes and wanted to wait for a specific process to terminate before we continued. 

### Write a program that creates a child process, and then in the child closes standard output (`STDOUT_FILENO`). What happens if the child calls `printf()` to print some output after closing the descriptor?

My assumption is that the parent process will be unable to print due to this context in the man page:

>The child process has its own copy of the parent's descriptors.  These descriptors reference the same underlying objects, so that, for instance, file pointers in file objects are shared between the child and the parent, so that an lseek(2) on a descriptor in the child process can affect a subsequent read or write by the parent.  This descriptor copying is also used by the shell to establish standard input and output for newly created processes as well as to set up pipes.

____

Turns out I was wrong, see:

> The close() call deletes a descriptor from the per-process object reference table.  If this is the last reference to the underlying object, the object will be deactivated.  For example, on the last close of a file the current seek pointer associated with the file is lost; on the last close of a socket(2) associated naming information and queued data are discarded; on the last close of a file holding an advisory lock the lock is released (see further flock(2)).

### Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the `pipe()` system call.

```c
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){
    int fds[2]; 
    pipe(fds);

    pid_t c1 = fork();
    if (c1 == 0) {
        dup2(fds[1], STDOUT_FILENO);
        close(fds[0]);
        close(fds[1]);
        //exec(...);
    }

    pid_t c2 = fork();
    if (c2 == 0) {
        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);
        close(fds[1]);
        //exec(...);
    }
    
    // wait(...);
    // wait(...);
}
```