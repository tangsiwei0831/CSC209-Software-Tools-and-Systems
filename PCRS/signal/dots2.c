#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* A signal handling function that simply prints
   a message to standard error. */
void handler(int code) {
    fprintf(stderr, "Signal %d caught\n", code);
}

int main() {
    // Declare a struct to be used by the sigaction function:
    struct sigaction newact;

    // Specify that we want the handler function to handle the
    // signal:
    newact.sa_handler = handler;

    // Use default flags:
    newact.sa_flags = 0;

    // Specify that we don't want any signals to be blocked during
    // execution of handler:
    sigemptyset(&newact.sa_mask);

    // Modify the signal table so that handler is called when
    // signal SIGINT is received:
    sigaction(SIGINT, &newact, NULL);

    // Keep the program executing long enough for users to send
    // a signal:
    int i = 0;
    
    for (;;) {
        if ((i++ % 50000000) == 0) {
            fprintf(stderr, ".");
        }
    }

    return 0;
}
// In the previous video we talked about how signals can be used to notify a process of an event.  And you learned that each signal has a default action associated with it.  There are times when we would like to be able to change the default behaviour of the signal - to print a message, save some state, or possibly ignore the signal.

// We can write a function to be executed when the signal is received by the process.  But how does this function get called? To answer this question we have to talk about what happens when a signal is delivered to a process.

// The process control block contains a signal table which is similar to the open file table.  Each entry in the signal table contains a pointer to code that will be executed when the operating system delivers the signal to the process. This is called the signal handling function.

// We can change the behaviour of a signal by installing a new signal handling function.  The sigaction system call will modify the signal table so that our function is called instead of the default action.

// The sigaction system call takes a little bit of explanation.  The first argument is the number of the signal that we are going to modify.

// The second argument is a pointer to struct that we need to intialize before we call sigaction.

// The third argument is also a pointer to a struct, but in this case the system call fills in the values of the struct with the current state of the signal handler before we change it.  We won't be using this argument in our examples, but it is sometimes useful to save the previous state of the signal.

// The first field of the sigaction struct is the function pointer for the signal handler function that we want to install. The remaining arguments we will get to later when we talk about blocking signals.

// Let's go back to our dot-printing example from the previous video.
// We will add the code to install the signal handler.

// First we need to write a function that will execute when the process is signalled. This is called the signal handler.

// We can select the name of course, but it needs to take a single integer
// parameter and have a void return type. For our example, our signal handler
// will just print a message to standard error.

// Now we need to add code to install our new function in the signal table.

// We create a struct sigaction and set its sa_handler member to be the handler function we just wrote.

// We will use the default flags and we set the sa_mask to empty so that no signals are blocked during the handler.

// At this point newact (our sigaction struct) is all set, and we have to
// call the sigaction system call to install handler for the SIGINT signal.

// Now lets try it out.

// We will compile and run our new program with the signal handler installed in this window.
// And here we will find the pid of the dots2 process and then send it a signal.

// When we send the INT signal to the process, we can see the new message printed to standard error. But the process doesn't terminate! That's because we didn't call exit in handler, so when the signal handling function finishes, control returns back to the process at the point where it was interrupted.

// If SIGINT isn't going to work to stop our process, how are we going to kill it?  We can send a different termination signal.

// Now the process has terminated.  But we could go install the same signal handling function for SIGQUIT by adding one more call to sigaction.

// Does this mean we could install a function for all the signals that cause the program to terminate, and create an unkillable process?

// Fortunately the designers thought of this possibility, and there are two signals that you can't change: SIGKILL and SIGSTOP.

// SIGKILL will always cause the process to terminate, and SIGSTOP will always suspend the process.

// In this video we have covered the basics of signal handling.  There are more details to learn if you need to write code that does more interesting things with signals, but you should now have a basic understanding of how signals work.