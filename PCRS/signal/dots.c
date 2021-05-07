#include <stdio.h>

/* Prints dots to standard error. */
int main() {
    int i = 0;

    for (;;) {
        if (( i++ % 50000000) == 0) {
            fprintf(stderr, ".");
        }
    }

    return 0;
}
// In this video we will talk about signals.  Before we get into the details, I want to show you some examples of signals that you have already seen.

// First, I'm going to run a program called dots that just prints out dots to standard error so that we can see that the program is running.

// It is running in the foreground, so I don't get the shell prompt back in the terminal window its attached to.

// I can send a termination signal to the process by holding down control and typing the letter C. We call that "control C".

// I'll run it again, but this time, I'll send a different signal by typing control-Z. This suspends the process. See, no more dots are being produced.

// To get it started again, I can run the "fg" program, which sends another signal to the process to wake it up. And then kill it with control C.

// Here is another example. I wrote a program called seg which deliberately dereferences a null pointer.

// When I run the program, it dereferences null and attempts to access an invalid memory location. When the operating discovers this, it sends a signal to the process to report it -- a segmentation fault -- and the process terminates.

// From the examples, you have seen that there are different types of signals, and different actions are taken when a signal is delivered to a process. In general, signals are a mechanism that allows a process or the operating system to interrupt a currently running process and notify it that an event has occurred. In the next video you will see how to write code so that you can determine what action to take when a particular signal arrives at your process, but for now, we will focus on how signals are implemented.

// Each signal is identified by a number between 1 and 31, and defined constants are used to give them names. Here is a list of signals from the man page for signals.  Each signal is associated with a default action that the process receiving
// the signal will normally perform when it receives the signal.

// When we type control C the terminal sends the SIGINT signal to the process and the default action is for the process to terminate.

// When we type control Z, the terminal sends the SIGSTOP signal to the process and the default action is for the process to suspend execution.

// The next question is how to send arbitrary signals to a process?  We can do it using a library function called "kill" or from the command line using a program also called kill.  Let's see how the kill program works:

// Let's run the dots program again. Then, in another window, we'll use kill to send signals.

// Before we can send a signal to a process, we need to know its process id. This time I will find out the process id by running ps.

// The number in the second column is the process id.

// Now I can use kill to send SIGSTOP. You can see that the dots have stopped printing, just like when I typed control Z. The dots process is now suspended.

// If I want to get it running again I can send it the SIGCONT signal.

// When we typed kill -STOP and the process id, we sent the SIGSTOP signal to the dots process. That's signal 17.

// We could have also have directed kill to send signal 17 directly.

// I'm pretty tired of seeing those dots, so let's make the process terminate by sending it a SIGINT signal.

// That's the signal we sent when we pressed control C.

// The library function kill provides the same functionality as the kill program, except that it is a C function. That means that inside your C code, you can send a signal to another process.

// Notice that to do that you would need to know the pid of the process that you are signalling. In most cases, we use signals to send messages to our children, so we can get the PID from the return value of fork. A child process could get the process id of its parent with getppid.