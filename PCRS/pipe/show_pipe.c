#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXSIZE 4096

/* A program to illustrate how pipe works.  The parent process reads
 * from stdin in a loop and writes each line it reads to the pipe.  The
 * child process reads from the pipe and prints each line is reads to
 * standard output.  Extra print statements are included so you can see
 * what each process is doing.
 *
 * Notes:
 *  - to close standard input from the shell type ctrl-D
 *  - In this example, we are writing the same number of bytes each time
 *    and reading the same number of bytes each time from the client.
 *    This simplifies the client side.
*/

int main() {
    char line[MAXSIZE];
    int fd[2];
    // create a pipe
    if (pipe(fd) == -1) {
        perror("pipe");
    }

    int r = fork();

    if (r > 0) {
        // Parent reads from stdin, and writes to child
        // close the read file descriptor since parent will write to pipe
        close(fd[0]);

        printf("Enter a line > ");
        while (fgets(line, MAXSIZE, stdin) != NULL) {

            printf("[%d] writing to pipe\n", getpid());
            if (write(fd[1], line, MAXSIZE) == -1) {
                perror("write to pipe");
            }

            printf("[%d] finished writing\n", getpid());
            printf("Enter a line > ");
        }

        close(fd[1]);
        printf("[%d] stdin has been closed, waiting for child\n", getpid());

        int status;
        if (wait(&status) != -1)  {
            if (WIFEXITED(status)) {
                printf("[%d] Child exited with %d\n", getpid(),
                        WEXITSTATUS(status));
            } else {
                printf("[%d] Child exited abnormally\n", getpid());
            }
        }

    } else if (r == 0) {
        close(fd[1]);
        printf("[%d] child\n", getpid());
        // Child will read from parent
        char other[MAXSIZE];

        while (read(fd[0], other, MAXSIZE) > 0) {
            printf("[%d] child received %s", getpid(), other);
        }
        printf("[%d] child finished reading", getpid());
        close(fd[0]);
        exit(0);

    } else {
        perror("fork");
        exit(1);
    }

    return 0;
}
// The fork system call gives us the ability to create multiple processes. If we can divide up a problem so that multiple processes can work on it simultaneously, then we should be able to solve these problems more quickly, and can take advantage of machines with multiple processors. But to cooperate to solve a problem, these processes need to communicate.

// Pipes are one type of communication mechanism that can be used to send data between related processes.  A pipe is specified by an array of two file descriptors, one for reading data from the pipe, and one for writing data to the pipe.

// When the program calls the pipe system call,

// The operating system creates the pipe data structures and opens two file descriptors on the pipe, one for reading and one for writing.  These two file descriptors are stored in fd, the array of two ints that we pass into pipe. The 0th element of the array is always the file descriptor used for reading from the pipe, and the file descriptor at index 1 is always used for writing to the pipe.

// After the pipe system call returns, the process can now read and write on the pipe.  But this isn't very useful because a process doesn't need a pipe to communicate with itself.

// However, we can now take advantage of a very useful aspect of fork.  When fork makes a copy of the exisiting process, it also makes a copy of all open file descriptors.  This means that the child process inherits all open files descriptors.

// After the fork call, the picture looks like this.  Both processes have the read and write file descriptors on the pipe open.  The picture looks a little messy, but we will clean it up in a moment.

// Pipes are uni-directional, one process will write to the pipe and another process will read from the pipe.

// So the parent can write to the pipe and the child can read from it,

// OR the child can write to the pipe and the parent can read from it.

// Once you have decided which direction the data should flow, you need to close the file descriptors that you won't be using. For example, if the parent will read data from the child, then

// we close the write file descriptor, fd[1], in the parent, and

// close the read file descriptor, fd[0], in the child.

// Now that the pipe is set up, we can use read and write system calls to send and receive data on the pipe.

// Here is a simple example that demonstrates that we can send text from the parent process to the child. It includes *most* of the error checking required. We should really be checking close, too.

// The parent uses fgets to read one line at a time from standard input and places each line in the pipe

// for its child to read.

// As we explained earlier, the parent closes the read end of the pipe, and the child closes the write end of the pipe. Now we're set to send data between the two processes.

// The parent writes the full character array of 256 bytes to the pipe.  This is somewhat wasteful because we are writing more data than we really need to. On the other hand, it simplifies the child process because it knows exactly how many bytes to expect, and doesn't have to worry about whether the strings are null terminated.

// The write command is wrapped in a loop, so one line is written at a time.

// Remember that fgets will return NULL if the file is closed, so the loop will exit when standard input is closed.  You can close standard input from the shell by typing control D.

// After the loop terminates, we close the write file descriptor for the pipe. This is important because when all the write file descriptors on the pipe are closed, a read on the pipe will return 0 indicating that there is no more data to read from the pipe.

// The child process uses this to detect when the parent is finished writing lines to the pipe. This loop terminates when the parent closes the write end of its file descriptor.

// The while loop will also terminate if an error occurs and read returns -1.  Because this is a simple example, we will leave that check out for now.

// Inside the loop, the child reads data from the pipe and displays it on the screen.

// One other note: the child process uses a different character array that is the same size as the one used in the parent process.  The only reason we didn't just reuse the "line" variable is to remind you that memory is not shared between processes.

// When the child has finished reading all the data from the pipe, we close its read file descriptor for the pipe, and exit.

// When a process exits, all of its open file descriptors are closed and all memory is freed, so it isn't strictly speaking necessary to close file descriptors before exiting, but it is a good habit to get into because it ensures you are thinking about which file descriptors are open.  In a long running program, it is important to close descriptors that are no longer needed because number of open file descriptors is limited, and it is possible to run out.

// The code for this program is posted, so please download it to try it out -- and to review the error checking we've included.