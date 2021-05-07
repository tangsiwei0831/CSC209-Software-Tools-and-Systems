#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

/* Demonstrate how file redirection is implemented using dup2.  */
// In this video you will learn how to redirect input or output from one file descriptor to another using the dup2 system call. Before we get into implementation details, let's talk about why you might want to do this. Let's take a look at the shell's redirection operators.

// Many useful shell programs read their input from standard input and write their output to standard output. For example, I could use grep to search for lines in a file that contain the phrase L0101 and print the output to standard output -- the screen.

// But printing output to the screen only lets us read it. One of the things that makes the shell so useful as a command-line interface is the ability to use redirection operators to send the output to different places without modifying a program.

// For example, the output redirection operator lets us save the output to a file.

// Redirection also lets us combine programs to perform complex tasks.

// Here, I use the pipe operator to use the output of grep as the input to the wc program.

// Now, let's think about the shell as a process. Consider this line.

// We know that grep is going to send its output to standard out, but the output redirection operator indicates that we want the output to go to a file, rather than to the screen. We don't want to modify grep to understand this operator. Instead, we need a way to change what standard output means so that when the grep program writes to standard output it *actually* goes to the file.

// The system call that we will need to do this is called dup2.  This system call makes a copy of an open file descriptor. We will use it to reset the stdout file descriptor so that writes to stdout will go to our output file.

// A file descriptor is really an index into a table.

// As we know from working with fork and pipes, each process has its own set of file descriptors. This means that each process has its own file descriptor table. This table is stored in the "process control block" and contains pointers to data structures that contain information about open files. For example, the 0 index into the file descriptor table usually contains a link to the console.

// Let's look at the problem we are trying to solve and break it down.  For the shell to execute a program, it first needs to call fork to create a new process. When a child process is created, it obtains a copy of the file descriptor table from its parent.

// Notice that even though the file descriptor tables are separate, the pointers in them may point to the same object. That's because file objects, like the console in this figure, are shared. Changes to the console will be observed by both processes.

// Later, when the child runs, it opens the file that will receive the output of the program.

// Note that we've opened the file with write permissions, so it can receive output.

// Now we can redirect standard output using dup2.

// We extract the file descriptor for stdout, a global variable of type FILE pointer, using the fileno function.

// After the dup2 call, file descriptor 2 points to the newly opened file, rather than the console.  Now when the process writes to stdout the output will be written to the file, rather than the console.

// It is a good idea to close the file descriptors that we are not using, so here we close filefd because we will not be writing to the file except as stdout.

// All these steps happen after the fork but before calling exec. Now, when the child process calls exec, all the file descriptors that were open before the exec call are retained, so we have now accomplished our goal. The shell program is running grep, but writes to stdout go to a file, rather than the console.

// The example code in redirect.c shows the implementation that we described in the video.  Try running it and see if you can see how it works.

int main() {
    int result;

    result = fork();
    
	/* The child process will call exec  */
	if (result == 0) {
        //int filefd = open("day.txt", O_RDWR | S_IRWXU | O_TRUNC);
        int filefd = open("day.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
		if (filefd == -1) {
			perror("open");
		}
        if (dup2(filefd, fileno(stdout)) == -1) {
			perror("dup2");
		}
		close(filefd);
		execlp("grep", "grep", "L0101", "student_list.txt", NULL);
		perror("exec");
		exit(1);

	} else if (result > 0) {
	    int status;
		printf("HERE\n");
		if (wait(&status) != -1) {
		    if (WIFEXITED(status)) {
			    fprintf(stderr, "Process exited with %d\n", 
				        WEXITSTATUS(status));
			} else {
			    fprintf(stderr, "Process teminated\n");
			}
		}
	   
	} else {
	    perror("fork");
		exit(1);
	}
   return 0;
}