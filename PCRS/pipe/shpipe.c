#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

/* equivalent to sort < file1 | uniq */
// In the previous video, we described how the shell implements redirection. Now, we'll walk through an example that uses pipe and dup2 to implement the shell's pipe operator. The shell pipe operator allows us to connect two processes so that the standard output from one process is the standard input for another process.

// The example that we will use for the remainder of the video uses the sort and uniq programs. Here's a sample file that shows how these programs work.

// The sort program sorts its input and writes it to standard output.

// Notice that it can operate on either standard input or, as in this case, files passed in as command-line arguments. For this example, we'll focus on the use of standard input and output.

// Uniq only emits lines that are different from the line that precedes them. In this case, only one "Pass" is displayed between T2 and T5, since the second one was cut.

// We would like to chain these programs so that only unique messages are printed -- regardless of their order in the original file.

// The program that we are walking through is in shpipe.c. You might want to download the file and follow along, pausing when you need some time to look at the code more closely. 

// The pictures we use in the example take a few short cuts so that we can fit everything on the screen. shpipe.c includes all of the error checking that we omit and also uses better variable names.

// Let's get started.

// In this picture you see the file descriptors used by the program and their initial state. When the program starts running, only the standard descriptors -- to the console -- are available.

// We really should be using the fileno function on stdin, or use the defined constant provided by the library. I couldn't fit either of these into my picture so I made up variables that would fit.

// The first step is to call the pipe system call, to create the pipe that will connect sort and uniq.

// Next we call fork.  Notice that the child process inherits all the same open file descriptors.  It is looking rather complicated at this point, but now our job is to set the file descriptors correctly, and close the ones that we do not need.

// Let's focus on the parent process first.  It will be executing sort, so we want standard input to come from the file file1.txt. We open the file for reading.

// And then we use dup2 to reset standard input so that the data will come from the file.  Now stdin is set correctly, so we will colour that link blue

// Now we close f because we won't be using that file descriptor directly.

// We also close fd[0] because this process will not be reading from the pipe.

// The next step is to connect stdout to the pipe so that we can send the output from sort to the input for uniq.  Since this is the final state that we want the stdout file descriptor to be in, we colour it blue.

// Now we close fd[1] because we won't be writing to the pipe using this file descriptor. This is very important because if we don't close all the write descriptors evenutally, the read end of the pipe won't know when there is nothing more to read from the pipe.

// The communication channels for the parent are now set up correctly, so we turn our attention to the child process.

// Here we connect stdin to the read end of the pipe, so that when the process reads from stdin the data comes from the pipe.

// We close fd[0] because we won't be using that file descriptor to read from the pipe.

// We also close fd[1] because this process will not be writing to the pipe.

// Now all of the file descriptors are correctly set up in the parent and the child processes.
int main() {
    int fd[2], r;

    /* Create the pipe */
    if ((pipe(fd)) == -1) {
        perror("pipe");
        exit(1);
    }

    if ((r = fork()) > 0) { // parent will run sort
        // Set up the redirection from file1 to stdin
        int filedes = open("file1", O_RDONLY);

        // Reset stdin so that when we read from stdin it comes from the file
        if ((dup2(filedes, fileno(stdin))) == -1) {
            perror("dup2");
            exit(1);
        }
        // Reset stdout so that when we write to stdout it goes to the pipe
        if ((dup2(fd[1], fileno(stdout))) == -1) {
            perror("dup2");
            exit(1);
        }

        // Parent won't be reading from pipe
        if ((close(fd[0])) == -1) {
            perror("close");
        }

        // Because writes go to stdout, noone should write to fd[1]
        if ((close(fd[1])) == -1) {
            perror("close");
        }

        // We won't be using filedes directly, so close it.
        if ((close(filedes)) == -1) {
            perror("close");
        }

        execl("/usr/bin/sort", "sort", (char *) 0);
        fprintf(stderr, "ERROR: exec should not return \n");

    } else if (r == 0) { // child will run uniq

        // Reset stdi so that it reads from the pipe
        if ((dup2(fd[0], fileno(stdin))) == -1) {
            perror("dup2");
            exit(1);
        }

        // This process will never write to the pipe.
        if ((close(fd[1])) == -1) {
            perror("close");
        }

        // SInce we rest stdin to read from the pipe, we don't need fd[0]
        if ((close(fd[0])) == -1) {
            perror("close");
        }

        execl("/usr/bin/uniq", "uniq", (char *) 0);
        fprintf(stderr, "ERROR: exec should not return \n");

    } else {
        perror("fork");
        exit(1);
    }
    return 0;
}
