#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXSIZE 4096
void handle_child1(int *fd);
void handle_child2(int *fd);

/* A program to illustrate the basic use of select.
 *
 * The parent forks two children with a pipe to read from each of them and then
 * reads first from child 1 followed by a read from child 2.
*/

int main() {
    char line[MAXSIZE];
    int pipe_child1[2], pipe_child2[2];

    // Before we fork, create a pipe for child 1
    if (pipe(pipe_child1) == -1) {
        perror("pipe");
    }

    int r = fork();
    if (r < 0) {
        perror("fork");
        exit(1);
    } else if (r == 0) {
        handle_child1(pipe_child1);
        exit(0);
    } else {
        // This is the parent. Fork another child,
        // but first close the write file descriptor to child 1
        close(pipe_child1[1]);
        // and make a pipe for the second child
        if (pipe(pipe_child2) == -1) {
            perror("pipe");
        }

        // Now fork the second child
        r = fork();
        if (r < 0) {
            perror("fork");
            exit(1);
        } else if (r == 0) {
            close(pipe_child1[0]);  // still open in parent and inherited
            handle_child2(pipe_child2);
            exit(0);
        } else {
            close(pipe_child2[1]);

            // This is now the parent with 2 children -- each with a pipe
            // from which the parent can read.

            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(pipe_child1[0], &read_fds);
            FD_SET(pipe_child2[0], &read_fds);

            int numfd;
            if (pipe_child1[0] > pipe_child2[0]) {
                numfd = pipe_child1[0] + 1;
            } else {
                numfd = pipe_child2[0] + 1;
            }

            if (select(numfd, &read_fds, NULL, NULL, NULL) == -1) {
                perror("select");
                exit(1);
            }

            // Read first from child 1
            if (FD_ISSET(pipe_child1[0], &read_fds)) {
                if ((r = read(pipe_child1[0], line, MAXSIZE)) < 0) {
                    perror("read");
                } else if (r == 0) {
                    printf("pipe from child 1 is closed\n");
                } else {
                    printf("Read %s from child 1\n", line);
                }
            }

            // Now read from child 2
            if (FD_ISSET(pipe_child2[0], &read_fds)) {
                if ((r = read(pipe_child2[0], line, MAXSIZE)) < 0) {
                    perror("read");
                } else if (r == 0) {
                    printf("pipe from child 2 is closed\n");
                } else {
                    printf("Read %s from child 2\n", line);
                }
            }
        }
        // could close all the pipes but since program is ending we will just let
        // them be closed automatically
    }
    return 0;
}

void handle_child1(int *fd) {
    close(fd[0]);  // we are only writing from child to parent
    printf("[%d] child\n", getpid());
    // Child will write to parent
    char message[10] = "HELLO DAD";
    write(fd[1], message, 10);
    close(fd[1]);
}

void handle_child2(int *fd) {
    close(fd[0]);  // we are only writing from child to parent
    printf("[%d] child\n", getpid());
    // Child will write to parent
    char message[10] = "Hi mom";
    write(fd[1], message, 10);
    close(fd[1]);
}
/*
In the last video we saw that reading from multiple sources with blocking read calls can lead us to wait indefinitely on one source while the other source has data available. In this video, we will learn how to avoid this situation by using a system call named select.

We will start with a simple select example, where we only make use of the first two parameters.

The basic idea with select is that the caller specifies a set of file descriptors to watch. The second parameter is the address of the set of descriptors from which the user wants to read.

Select blocks until one of these file descriptors has data to be read or until the resource has been closed. In either case, the user is certain now that calling read on that file descriptor will not cause read to block. We will say that a file descriptor like this (with data or with a closed resource) is "ready".

Select modifies the descriptor set so that when it returns, the set only contains the file descriptors that are ready for reading. It might be the case that more than one file descriptor is ready when select returns.

Let's add select to the code we used in the last video.

Remember that the code before this set up 1 parent and 2 children where each child had a pipe to write to the parent.

We insert our select call here -- before calling read on either child. We check the return code and call perror if we detect a problem.

Earlier, I said that read_fds was a *set* of descriptors to be watched. We need to declare and initialize this set before calling select.

We'll declare a variable of type "fd_set" -- for "file descriptor set". An fdset is implemented as a bit field stored in an array of integers. You can learn more about this implentation by watching the videos on "Bit Manipulation and Flags".

But for our purposes, you don't have to understand the implementation details. Think about a descriptor set as a mathematical set.

This line declares an fdset but as with other C variables, it doesn't initialize the value.

Macros have been provided to perform standard set operations -- like inserting an item into the set, removing an item from the set, or checking if an item is a member of the set.

This macro initializes the set of descriptors to be empty.

And these two lines add the file descriptors for the read ends of the two pipes into the formerly-empty set. Now we have a set that contains exactly these two file descriptors.

And now our read_fds parameter is ready for use by the select call.

But what about the first parameter? Select's first parameter is a bit confusing. What you need to do is set this parameter to be the value of the highest file descriptor in your set *plus one*.

Weird, eh? It's for efficiency and for historical reasons. Even though an fd_set could represent lots of descriptors, most of them are not currently members of the set. Select can be implemented more efficiently if we specify that our set can only contains file descriptors between 0 and numfd - 1.

So in our case we add one to the larger of the two file descriptors. And now our select statement is ready for use.

But how does the call send a response back to us? In our code we've used the return value only for error checking.
It is set to the number of file descriptors that are ready, but doesn't tell us which ones. 

Notice that we sent the *address* of the file-descriptor set to select. That's because select modifies read_fds.

Remember that select will block until one (or more) of the descriptors in the set is ready. When select does finally return, the descriptor set read_fds will ONLY contain the file descriptors that are ready.

So we need to use another set operation -- checking set membership -- to determine which descriptors remain in the set.

The macro FD_ISSET takes a single fd and returns whether or not it is in the fdset pointed to by the second parameter.

So in our case, we want to know if the pipe from child 1 is in read_fds after the select call. If it *is* in the set, then we know that this pipe is ready, so we can read from it without fear of blocking.

We add a similar check to see if the file descriptor from child 2's pipe is ready and then read from it.

Note that this is an independent if statement. It is possible that after select returns, BOTH pipes are ready for reading.

That's enough to demonstrate the basic use of select, but we should say something about those other parameters that we set to NULL.

Like the second parameter, the third and fourth are also file descriptor sets. You can use these sets to check which file descriptors are ready for writing or have error conditions pending, respectively.

The final parameter is a pointer to struct timeval. You can use it to set a time limit on how long select will block before returning (even if no file descriptors are ready.) You can use this, for example, to interrupt the blocked select call after some number of seconds to do some other task. Then you could call select again and resume watching the file descriptors for activity.

One other issue: read_fds is modified by select, so we can't just use it again in a second select call. If we want to read from our pipes multiple times, in a loop, we'll need to re-initialize the set. In the next video, we'll take a deeper look at how fd_sets are implemented and discuss how to use select within a loop.
*/