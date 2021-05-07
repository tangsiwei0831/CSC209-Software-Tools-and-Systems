#include <stdio.h>
#include <stdlib.h>
// In these modules so far, the only IO operations we have been using are those that operate on streams using the FILE objects.  These functions include fopen, fclose, printf, fgets, fwrite, and so on.  These functions are great to use with files because they hide some of the complexity of the actual IO system calls.

// In particular, they buffer data. That is, they may read or write larger blocks of data than the user has specified. Collecting data into larger blocks allows the file system to decide exactly when to send data to the disk, network or screen, and to amortize the cost of data transfer by reducing the number of system calls made. Delegating these tasks allows the programmer, in most cases, to ignore the performance details of data transfer.

// For example, let's look at a simple program that opens a file and makes a few fprintf calls to write some text to the file.

// Note that fopen returns a pointer to a FILE struct. The FILE struct holds all the data that the file system code needs to implement buffering.

// This program contains five separate fprintf calls, and these calls ask a question.

// In order for the data to be written to disk, a system call like write must happen somewhere in the library code.  On Linux we can use a program called strace to run our program and see which system calls are made. This will let us determine how many write calls are generated.

// strace spits out a lot of output because even a simple program makes a number of system calls.

// In all of this output, only a single write call is generated.

// That call writes 84 bytes -- the length of the string produced by all five of our printf calls.

// But what is the value "3" in that system call?

// It's a "file descriptor" -- an integer that represents an open file, or open communication channel.  File descriptors are of type int because the operating system uses them as indexes into a table of open files.

// But why "3" in particular? The file we write to is assigned the file descriptor three, rather than 0 or 1, because three files are automatically opened for you: stdin, stdout, and stderr.

// We will run low_level in the debugger to see what these variables look like.  These global variables are actually of type FILE pointer, but if we display the contents the FILE struct, you can see the file descriptor.  stdin has file descriptor 0, stdout file descriptor 1, and stderr has file descriptor 2.  We will follow good programming practice and not use hard-coded numbers in our programs, but since these numbers come up pretty often, it is useful to know where they come from.

// Knowing the value of a file descriptor is sometimes useful for debugging, but in general, you should not have to look at the value of the file descriptor. Think of the value returned by fopen -- or the low-level open system call -- as a name that is required when performing I/O.

// In the next video, we will begin to look at pipes -- a form of interprocess communication that also uses file descriptors. Although open is used to open a file, and pipe is used to set up a communication channel, they both operate on file descriptors. This is convenient because the same read and write system calls can be used both for files and pipes.

int main() {

    FILE *outfp = fopen("tmpfile", "w");

    if(outfp == NULL) {
        perror("fopen");
        exit(1);
    }

    fprintf(outfp, "This is ");
    fprintf(outfp, "one of several ");
    fprintf(outfp, "calls to fprintf.\n");
    fprintf(outfp, "How many write ");
    fprintf(outfp, "system calls are generated?\n");
    fclose(outfp);
    return 0;
}