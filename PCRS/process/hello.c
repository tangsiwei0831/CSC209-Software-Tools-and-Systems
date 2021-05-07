#include <stdio.h>
#include <unistd.h>
// In this module, you have been learning about the system calls the operating system provides to create and manage processes. You have already learned about fork, which allows you to create a new child process. The parent can wait for the child to terminate and collect its exit value.  This is useful all on its own, but the parent and child processes are both running the same program. The next step is to load and execute a different program.

// Linux provides the "exec" family of functions to replace the currently running process with a different executable.  While there are several variants on the exec function, they all perform the same task. They just differ in how the arguments to the function are interpreted.

// Let's start with a trivial example. The do_exec program prints a message and then calls execl.  The first argument to execl is the path to an executable.  The remaining arguments to execl are the command line arguments to the executable named in the first argument. In this case, we don't have any commandline arguments.  However, execl requires that we include NULL as the final argument.

// When the process calls execl, control is passed to the operating system, so let's look at the operating system's view of this process.  The code is loaded into the code region of the address space -- but as machine executable instructions, not source code like the picture. The program counter points to the execl function, and the stack pointer points to the main function stack frame since that is the function we are currently executing.

// When performing execl, the operating system finds the file containing the executable program and loads it into memory where the code segment is. It also initializes a new stack. The program counter and stack pointer are updated so that the next instruction to execute when this process returns to user level is the first instruction in the new program.

// In our example, the executable that is passed into execl is a simple hello world program.  Note that we are passing in the name of a compiled program, not source code.

// Here is an idea that takes a bit of getting used to. When control returns to the user level process, the original code that called exec is *gone*, so it should never return. That means that these lines should never execute. However, execl will return if an error occurs and it wasn't able to load the program, so it is still worth checking for errors.

// Remember: the operating system does not create a new process -- that's a job for fork. Instead, exec asks the operating system to modify the calling process.

// This means that the process has the same process id after exec, and as we will see when we talk about pipe, it retains some state from the original process such as open file descriptors.

// Earlier, we mentioned that exec was a family of functions. Here is a man page that describes them. They are used in similar ways -- just like in our example -- but you have to pick the one that you need. There are quite a few variants, but here is a mnemonic to help you remember what they do. Look at the letters that follow "exec" in the names of these functions.

// l stands for list, so the command line arguments to the program are passed as a list of arguments to the exec function.

// v stands for vector, so the command line arguments are passed in as an array of strings, just as we see with argv parameter of main.

// p stands for path, which means that the PATH environment variable is searched to find the executable.

// Without the p, execl and execv expect that the first argument is a full path to the executable.

// e stands for environment.  You can pass in an array of environment variables and their values so that the program executes within a specific enivironment.

// You will find yourself using execvp or execlp most often.

// One other thing before we stop talking about exec.

// So, now we can fully explain how the shell executes the commands we enter. Based on what you've learned in this module, can you explain it?

// The shell is just a process, and it uses fork and exec just as we have described in these videos.

// When you type a command at a shell prompt, the shell first calls fork to create a new process, and then the child process calls exec to load a different program into the memory of the child process. Typically, the shell process then calls wait, and blocks until the child process finishes executing. When the wait call returns, it prints a prompt indicating it is ready to receive the next command.
int main() {
    printf("Hello.  My PID is %d\n", getpid());

    return 0;
}