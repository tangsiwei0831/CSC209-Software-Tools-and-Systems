#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// In the last video, we defined the idea of a process, but we didn't cover how to create them. 
// In this video, we will learn how to create a new process using the *fork* system call. 
// Creating a new process requires a system call because the operating system must set up data structures,
//  such as the process control block, needed by the new process.

// When a process calls fork, it passes control to the operating system.

// The operating system make a copy of the existing process.

// To duplicate a process, the operating system copies the original process's address space -- 
// its data -- and the process control block that keeps track of the current state of the process. 
// As a result, the newly created process is running the same code, has the same values for all variables in memory,
//  and has the same value for program counter and stack pointer. They're almost identical.

// However, there are two important differences:  The newly created process gets a different process id (PID), and the return value of fork is different in the original process and the newly created process.

// The original process and the newly created copy are related.  We call the original process the parent process, and when a process calls fork, a child process is created.

// The child process has the same value for the program counter as the original process, so when the child process runs, it starts executing just after fork returns. However, we don't know whether the parent process or the child process will execute first. When the operating system is finished creating the new process, control can be returned to either the parent *or* the child.

// Let's go through a simple example in code, so you can see what happens step by step. Throughout the example, we assign to and print the variable i to get a sense of the flow through the program.

// Suppose the original process has process id 677.  It starts executing as you would expect by setting i to a value of 5, and then printing 5.  Then, when it makes the fork call, it is as if time stops while the operating system does a bunch of work behind the scenes to create the child process.

// The child process is given a new process id -- 680 in this case, and it begins executing at the point when fork returns. The variable i still has the value 5 because the new process is a copy of the old one, but neither the assignment statement nor the printf statement are executed by the child process.

// The return value of fork in the parent process is the process id of the child.
// The return value of fork in the child process is 0.

// If fork fails, then the return value of fork in the parent is -1 and the new process is not created.  Fork might fail if there are already too many running processes for the user or across the whole system.

// From the programmer's point of view we now have two processes ready to execute exactly the same code. The return value of fork allows us to distinguish between the two processes, so that the parent process can follow a different path through the code than the child process.

// In this example, the parent process now sets i to be i + 2, or 7, and the child process sets i to i - 2, or 3.

// If we run the program, the output is 5 7 3.  The order of the output could also be 5 3 7 because we don't know whether the parent process or the child process will get run on the CPU.  The operating system controls the order that processes run.

// The other really important thing to notice is that the parent and child processes are now completely separate processes and don't share memory.  In our example, when the parent sets i to i + 2 the value of i in the child process does not change -- because the child has its own memory and its own variable i.

// In the next videos, we will explore the relationships between processes, and how they can interact.
int main() {
	int i; 
	pid_t result;

	i = 5;
	printf("%d\n", i); 

	result = fork();

	if (result > 0) {
		i = i + 2; 		// parent process
	} else if (result == 0) {
		i = i - 2;		// child process
	} else {
		perror("fork");
	}

	printf("%d\n", i);
	return 0;
}