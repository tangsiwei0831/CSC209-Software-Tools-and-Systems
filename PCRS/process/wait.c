#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
// In the previous video, we saw evidence that the shell process was waiting for our process to terminate -- 
// but we didn't see the same behaviour in our own program. The difference is explained by the wait system call. 
// The program we run from the shell is a child of the shell process. The shell uses the wait system call to suspend itself 
// until its child terminates. In this video, we will look more closely at the relationship between the shell and processes it spawns.

// We can use the wait system call to force the parent process to wait until its children have terminated.  The man page for wait tells us that it

// "suspends execution of the calling process until one of its children terminates." Information about how the child terminated -- cleanly or with a particular error -- is stored in the int passed in using the stat_loc argument.

// We have modified the example code from the previous video that spawned 5 child processes. Now, the parent calls wait, so it will block until all 5 child processes have terminated.

// Here are the changes. We must call wait once for each child that was created to wait for all of the child processes.

// The information in the status argument is only useful if the system call succeeded, so we must check wait's return value before using it. If wait returns successfully, its return value is the process id of the child that terminated. If wait fails, it returns -1. This code stores the return value of wait into the variable pid and then checks if the value is a real pid or -1.

// When we compile and run the program, we can see that the parent doesn't terminate until all the child processes have terminated. The children may or may not terminate in the same order as they were created. In this case, they do.

// When a program calls exit,

// as we see in the error checking for fork,

// or calls return from the main function,

// then we provide an argument like -1 or 0. This value makes up part of the status value in wait.

// This exit value enables limited communication from the child back to the parent. By convention, a status of 0 represents a successful run of the process, and non-zero values represent various abnormal terminations. But the exit value is only part of the value of the status returned by wait.

// Let's modify our example program so that each child process exits with a different value. We print the value of status after we call wait.

// Let's see what happens when we compile and run.  The values may not be what you expect!

// The status isn't just the argument to the exit function. Various bits in the status argument are divided up and used for different purposes. In particular, the lowest 8 bits tell us whether the child process terminated normally, or whether it terminated because it received a signal (like control C). If it terminated due to a signal, the lower 8 bits tell us which signal.

// The exit value of the child process is in the next 8 bits, which is why the process that exited with a 1 has status 256 (2 to the eight) and why the process that exited with a 2 has status 512 (2 to the eight times 2).

// The man page for wait tells us how to extract the values we want from the stat_loc argument, and it notes that several macros are provided to help you extract the data you want. As a programmer, you shouldn't need to know exactly how the bits in the stat_loc argument are used; you just need to use the macros correctly.

// Let's do that.

// First, we need to add an include so that the macros are available for use in our program.

// Next, we use the macro WIFEXITED to check if the process terminated normally. If it did, then we use WEXITSTATUS to extract the exit value of the process.

// Otherwise, if a process exits as a result of a signal, then we use WIFSIGNALED and WTERMSIG to find out the number of the signal that caused the process to terminate.

// And just to be careful, we make sure that we're handling all of the cases.

// Finally, we force child 2 to exit abnormally by calling abort instead of exit, just so that we can see what happens when we wait for a process that terminates abnormally.


// We see that child 2 does terminate with a signal. The signal for abort is 6.

// If we need a bit more control, the waitpid system call lets you specify which child process to wait for. To do so, you pass in the process id of a child process.

// You can also pass in the WNOHANG option to waitpid if the parent process just wants to check if a child has terminated, but doesn't want to block.

// But both wait and waitpid have a limitation: they only wait for child processes. You cannot wait for an unrelated process or even a child of a child process. wait and waitpid only give us a *limited* form of synchronization between processes.

// Nevertheless, waiting for the termination of a child is a common operation in multiprocess programs. For more powerful communication between processes, you can use a pipe or signal.
int main() {
    int result;
    int i, j;

    printf("[%d] Original process (my parent is %d)\n",
            getpid(), getppid());

    for (i = 0; i < 5; i++) {
        result = fork();

        if (result == -1) {
            perror("fork:");
            exit(1);
        } else if (result == 0) { //child process
            for(j = 0; j < 5; j++) {
                printf("[%d] Child %d %d\n", getpid(), i, j);
                usleep(100);
            }

            if(i == 2) {
                abort();
            }
            exit(i);
        }
    }
	sleep(10);
    for (i = 0; i < 5; i++) {
        pid_t pid;
        int status;
        if( (pid = wait(&status)) == -1) {
            perror("wait");
        } else {
            if (WIFEXITED(status)) {
                printf("Child %d terminated with %d\n",
                    pid, WEXITSTATUS(status));
            } else if(WIFSIGNALED(status)){
                printf("Child %d terminated with signal %d\n",
                    pid, WTERMSIG(status));
            } else {
                printf("Shouldn't get here\n");
            }
        }
    }
    printf("[%d] Parent about to terminate\n", getpid());
    return 0;

}