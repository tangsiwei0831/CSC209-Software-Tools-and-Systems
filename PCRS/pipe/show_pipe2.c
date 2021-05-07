#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXSIZE 8192

/* A program to illustrate how pipe works.  The parent process reads
 * from stdin in a loop and writes each line it reads to the pipe.  The
 * child process reads from the pipe and prints each line is reads to
 * standard output.  Extra print statements are included so you can see
 * what each process is doing.
 *
 * Notes:
 *  - to close standard input from the shell type ctrl-D
 *  - You will save yourself some grief if you read the same number of bytes
 *  from the pipe as were written to the pipe. This is especially important
 *  if you are working with binary data.
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
            sleep(2);
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
// In a previous video, we covered the basics of pipe operation. But we didn't explain some important details of how pipes really work. Pipes are used to communicate between two independent running processes. The operating system decides when these processes run, so it is possible -- even likely -- that they won't run in lock step with each other.

// In fact, this is an example of the more general Producer/Consumer problem.  In the Producer/Consumer problem,

// one process is producing or writing data,

// and the other process is consuming or reading data. They are connected by a queue.

// The producer and consumer may not work at the same rate. If the producer is running and the consumer is not -- or if the consumer is not running enough to keep up -- then data can pile up. This is a problem if we have limited buffer space in the queue. We don't want the producer to try to put data into a full queue.

// On the other hand, if the consumer is running but the producer is not, then the consumer will not have data to operate on. In this case, we need to make sure that the consumer doesn't try to remove data from an empty queue -- or from a queue where the producer is just starting to add data.

// Here are the three cases we're worried about. Let's see how the producer-consumer problem applies to pipes.

// The pipe is a queue data structure in the operating system. The process writing to the pipe is the producer, and the process reading from the pipe is a consumer.

// Since the operating system manages the pipe data structure, it ensures that only one process is modifying it at a time. This eliminates the case where a consumer removes data as a producer is writing it.

// Next, let's consider what happens if the producer takes longer to write data to the pipe than the consumer can read it.  The consumer process will call read on the pipe when it contains no data.

// Fortunately, the operating system helps us out in this case.  The read call will block if the pipe is empty.

// You have already seen an example of this in a previous video. The parent process reads from standard input and writes to the pipe. The child process has very little work to do. It just reads from the pipe and writes a message to standard output.

// The parent has to wait for the user to type something, which means that

// the child process spends quite a bit of its time blocked, waiting for the parent to write data to the pipe.

// Finally, let's consider the last case -- when the producer operates more quickly than the consumer. The pipe will eventually fill up and

// to prevent data from being lost, the operating system will cause a write to the pipe to block until there is space in the pipe.

// We can watch this in action by putting a sleep statement in the child to slow down its reading. Now the parent process can write multiple times before the child reads.

// I'm going make sure the input is fast by redirecting it from a file so the process doesn't have to wait for me to type each line of input. And I'm going to make sure the pipe gets filled by writing 8192 bytes each time.

// We can see that the parent process eventually has to wait until the child reads each chunk of data from the pipe before it can write its next piece of data.