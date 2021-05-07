#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
// The parent-child relationship between processes leads to some interesting issues.  In a previous video, we discussed how a parent process can wait for a child to terminate and noted that a process can only wait for its direct children. This raises the question of what happens when a child process terminates before the parent calls wait.

// The example illustrates this.  In this program the original process calls fork once.  The parent process sleeps for 20 seconds before it calls wait. The child process prints only one line and then exits. If we are quick, we can use top to see the state of the child process before the parent process calls wait.

// The state of the child process is given as Z and the process is shown as "defunct". The Z stands for zombie, which may sound funny, but is a good analogy for the state of the process.  The child process has called exit, so it is dead ... but not quite.  The operating system needs to keep the exit information of the process somewhere in case the parent calls wait to get this value.  So the operating system can't delete the process control block of the terminated process until it knows it is safe to clean it up.  This means that a zombie process is a process that is dead, but is still hanging around for the parent to collect its termination status.

// But what happens if the parent never calls wait? Why don't we have dozens or hundreds of zombie processes using up the process table?

// Before we answer that question, let's look at another question:  what happens to a child process when its parent terminates? Naturally, we call the child process an "orphan" when its parent terminates first. But who is the parent of the orphan process? Does it even have a parent?

// Here's a variant of the program we used in the video about fork.

// We have added a call to getppid -- a system call that gets the parent process's ID -- to the child processes' print statement.

// Note that the parent process is *not* waiting after the child processes are created by fork.

// Due to usleep, the child processes will likely still be active after the parent terminates, so this means that we'll see what getppid reports when the child processes are orphans.

// When we run the program we see that the first statements printed by the children have the correct parent process id, but shortly after the parent process prints the message that it is about to terminate, the parent process ids of all the running children become the value 1.

// The process with process id 1 is the "init" process -- the first process that the operating system launches. When a process becomes an orphan, it is "adopted" by the init process.

// Now we can come back to the question of when zombie processes go away. A zombie is exorcised -- put to rest -- when its termination status has been collected. The main task of the init process is to call wait in a loop, which collects the termination status of any process that it has adopted. After init has collected the termination status of an orphaned process, all of the process's data structures can be deleted, and the zombie disappears.
int main() {
    int result;
    int i, j;

    printf("[%d] Original process (my parent is %d)\n",
            getpid(), getppid());

    for(i = 0; i < 5; i++) {
        result = fork();

        if(result == -1) {
            perror("fork:");
            exit(1);
        } else if (result == 0) { //child process
            for(j = 0; j < 5; j++) {
                printf("[%d] Child %d %d (parent pid = %d)\n",
                        getpid(), i, j, getppid());
                usleep(100);
            }
            exit(0);
        }
    }
    printf("[%d] Parent about to terminate\n", getpid());
    return 0;

}