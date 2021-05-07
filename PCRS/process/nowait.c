#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
// So far, you have learned how to create new processes using fork, and how to set up your program so that the parent and child processes execute different parts of the program. In this video, we will look more closely at the parent/child relationship, and what happens when a process terminates.

// Recall that the programmer can't control whether the child process or parent process executes first after they return from fork. Therefore, if both the child and the parent are producing output, the order of the output may be different each time we run the program.

// Here is a program that creates several processes. Let's look at how it works.

// In this program, the original process prints a message that includes its PID and its parents PID.

// It creates 5 child processes and then prints a message and terminates.

// Each child process executes 5 iterations of a loop.

// In the body of the loop, it prints a message and then sleeps for a short amount of time. The usleep system call is there to slow the processes down enough to see what can happen when they run concurrently.  It also introduces more variability in how the processes run because every time usleep is called, control is passed to the operating system, which gives the operating system the opportunity to make a scheduling decision -- to decide which process will run next.

// Before I run this -- scribble down what you expect to see on a piece of paper.

// The output may not be what you expect.  The first line of output is "original process" which is printed before the program calls fork. Then we see some lines printed by child processes, but before all the children have printed their first message, the parent prints the "about to terminate" message. Then more child processes print lines.

// If you look closely at the output, you can confirm that each child process does print the 5 lines of output that we expect each process to print. The number in the square brackets is the process id, and all messages printed by one child have the same process id.

// For example, here is the output for child 2. Notice that the order of the output for this child process -- and the others, too -- is exactly what you would expect of a single process. They're in the right order.

// But the print statements for child 2 and child 0 are not interleaved in a predictable way.

// Also, why does the parent output come before so much of the child output?

// Let's look at what the parent process does in the code.  After printing the first message, it calls fork 5 times, but it doesn't have anything else to do in the body of the outer for loop, so after it completes its 5 iterations, it prints the final message and the process terminates.

// From the operating system's point of view, the parent process is no different than any of the child processes, so the operating system schedules the parent process to run the same as it does for the child processes. There's no reason for it wait for its children to execute, for example.

// Now what about that shell prompt?  Why do we see output after the shell prompt appears? And why don't we see a shell prompt when all the output has finished? Normally, when you run a program the shell waits until the process has finished and then prints a prompt for the next command.

// That is happening here too.  The shell waits for the original parent process to finish, and then prints a shell prompt.  But the shell is just another process that the operating system has to schedule, so a few child processes get to print some output before the shell prints its prompt. Once the shell runs, it prints it message -- but some child processes haven't finished yet, so they print more output afterwards.

// Because we don't see the shell prompt at the end of the output, it looks like the program hasn't finished yet, or is "hanging".  But in fact, if I hit enter

// I see the prompt show up again. The program has finished, and it was waiting for our input. If we type in a command like ls,

// it will work as expected.

// So, one final question: We've seen lots of processes executing without regard for others, but how does the shell know to wait for the parent process?

// There must be a way for one process to *wait* for another. We'll look at this in the next video.
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
        } else if (result == 0) {   //child process
            for (j = 0; j < 5; j++) {
                printf("[%d] Child %d %d\n", getpid(), i, j);
                usleep(100);
            }
            exit(i)
        }
    }

    printf("[%d] Parent about to terminate\n", getpid());
    return 0;
}