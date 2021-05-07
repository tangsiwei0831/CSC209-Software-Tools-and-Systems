#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXSIZE 4096
void handle_child1(int *fd);
void handle_child2(int *fd);

/* A program to illustrate the need for select.
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
            //  from which the parent can read.

            // Read first from child 1
            if ((r = read(pipe_child1[0], line, MAXSIZE)) < 0) {
                perror("read");
            } else if (r == 0) {
                printf("pipe from child 1 is closed\n");
            } else {
                printf("Read %s from child 1\n", line);
            }

            // Now read from child 2
            if ((r = read(pipe_child2[0], line, MAXSIZE)) < 0) {
                perror("read");
            } else if (r == 0) {
                printf("pipe from child 2 is closed\n");
            } else {
                printf("Read %s from child 2\n", line);
            } 
        }
        // We could close all the pipes but since program is ending, we will just let
        // them be closed automatically.
    }
    return 0;
}

void handle_child1(int *fd) {
        close(fd[0]);  // we are only writing from child to parent
        printf("[%d] child\n", getpid());
        // Child will write to parent

        // Uncommenting the following while loop will show how child2's written
        // message can be *blocked* by child1:
        
        // while (1) {
        //     // do something
        // } 
        

        char message[10] = "HELLO DAD";
        write(fd[1], message, 10); 
        close(fd[1]);
}
void handle_child2(int *fd) {
        close(fd[0]);  // we are only writing from child to parent
        printf("[%d] child\n", getpid());
        // Child will write to parent
        char message[10] = "Hi mom";

        // This written message will never be processed (read by the parent) 
        // if child1 blocks:
        write(fd[1], message, 10);

        printf("[%d] child is finished writing\n", getpid());
        close(fd[1]);
}
/*
At this point, we've learned quite a few C systems calls. You've seen that some calls
(like read or accept or even write) can block and wait for something else to happen before they
can complete their task. In this video we will illustrate a problem with using blocking calls to 
read from multiple sources and see the need for another system call that can let us know which 
of those sources are ready for our attention.

Suppose we have a parent process that has forked a child. Before forking, the parent correctly set up a pipe from the child to the parent. Now the parent would like to read from the child.

If the parent calls read on the child's pipe, and that child hasn't written anything to the pipe yet, then the read system call blocks. That means that the parent process doesn't go on to execute the next instruction but sits and waits for the child to write something. 

When the child writes some bytes to the pipe, the read call returns.  The parent checks the return value to determine that the read call succeeded.

The parent goes on to execute the next instruction.  In this case, it is another read call on the pipe. This read call will block until the child writes something.

This time, the child closes it's end of the pipe, and then the read returns and indicates the fact that the pipe is closed with a return value of 0. 

Got that? read on a pipe blocks until there is something to read or until the other end of the pipe is closed. Then, *when* there is action on the pipe (either something to read or the pipe *gets* closed, read returns and the program continues.

Now, let's change the scenario to have two children with a shared parent where each child has a pipe to the parent. Our parent process would like to read from both childen -- or really from EITHER child -- whichever is ready. 

We need to have two different calls to read: one to read from child 1, and one to read from child 2.

In fact, we probably want to have multiple read calls to each child since children usually have a lot to say.

Suppose we put them in this order. First read from child 1 then read from child 2. Then put the whole thing into a loop so we can read lots. This works fine if child 1 is ready first, and everytime child 1 writes, child 2 also writes.

But what if child 2 has lots to say and child 1 has nothing to say?

The parent will block on the read from child 1 and wait ... and wait ... and wait ...

That's fine if child 2 didn't have anything to say either. But child 2 has lots and lots to say, so much perhaps that the pipe to the parent might get full. Then child 2's writes will block. Now BOTH child 2 and the parent are blocked. 

Maybe it would be better if we reorganized the parent code to read first from child 2?

Now the parent reads first from child 2 and then child 1. But as soon as one read to child 2 is complete, it is time to read from child 1 again. And we are back to the same problem.

No matter how we order the read calls, we can always get into a situation where the parent is blocked waiting to read from one child, while the *other* child has data ready to be read. 

Let's quickly walk through some code that illustrates this problem.

The full version of this code is available in PCRS. I recommend you download it and play with it.

We are going to fork two children, and each time we will check for errors and respond accordingly. Before we fork each child, we have to create a pipe -- and check for errors of course! And here is where we declared the pipe file descriptors that we needed for the pipe calls.

And like we learned in the pipe videos, we need to close the ends of the various pipes that we are not using. 

So, all the code above here is to set up the situation where we have 1 parent with 2 children
and one open pipe from each child to the parent. The situation from our earlier picture.

Now, we will read one time from child 1 and print out what we read. And then we will do the same thing for child 2. Make one read call and then print out what we read.

So now let's add code to do something as the children.

We close the reading end of the pipe, print a message to standard out with the child's pid so we can see that the child is running, and finally close the pipe.

handle_child2 will be almost the same for now. We'll change the function name and change the message.

Now, when we compile and run the code we always read the message to DAD first. We don't know in which order the children actually ran, but we always make child2 wait and read first from child 1.

Let's add some code to child 1 to keep him busy for a while before he writes. In fact, let's keep him busy forever with an infinite loop. So he doesn't write anything to the pipe, but he doesn't close it either. And before we finish editing, let's add one more line to child 2 just to confirm that this child has done the write.

We'll compile again and run... and we see that both children are executing but the parent is waiting. Waiting on child1 to write something to the pipe. Child2 has long since written the "hi mom" message, but the parent isn't reading from child 2. 

This is a problem! No matter how we order the read calls, we can't be guaranteed to not be waiting for one child while the other child has data ready for us to read. 

In the next video we will introduce the select system call. Select lets us specify a set of file descriptors and then blocks until at least one of them is ready for attention. Then select tells us which file descriptors are ready so we can avoid blocking on the non-ready ones. 
*/