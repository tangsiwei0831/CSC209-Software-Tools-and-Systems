#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <arpa/inet.h>     /* only needed on my mac */

int main() {
    // create socket
    int listen_soc = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_soc == -1) {
        perror("server: socket");
        exit(1);
    }


    //initialize server address    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(54321);  
    memset(&server.sin_zero, 0, 8);
    server.sin_addr.s_addr = INADDR_ANY;

    // bind socket to an address
    if (bind(listen_soc, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) == -1) {
      perror("server: bind");
      close(listen_soc);
      exit(1);
    } 


    // Set up a queue in the kernel to hold pending connections.
    if (listen(listen_soc, 5) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }
   
    struct sockaddr_in client_addr;
    unsigned int client_len = sizeof(struct sockaddr_in);
    client_addr.sin_family = AF_INET;

    int client_socket = accept(listen_soc, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("accept");
        return -1;
    } 


    write(client_socket, "hello\r\n", 7);

    char line[10];
    read(client_socket, line, 10);
    /* before we can use line in a printf statement, ensure it is a string */
    line[9] = '\0';
    printf("I read %s\n", line);

    return 0;
}

// In our first three videos in our series about sockets, we saw how to set up
// a stream socket between a server and a client running on two different machines.
// Today, we will demonstrate how to use that connection to communicate between
// the two programs.

// Here is a representation of the code we have been developing. Let's review what
// happens in the server.

// The socket system call is used to create a socket on which the server will
// listen for connections.

// We bind (or assign) that socket to a particular port and the address of the machine on which we are running the server.

// we tell the socket to start listening for partial connections

// And then we call accept, which blocks, returning only if there is an error
// or when a connection is made.

// Remember: when accept returns, it returns the descriptor for a new socket.
// We'll use this socket to communicate with the client.

// I'm going to change the name of this variable to client_socket to better
// represent this.

// So, what happens to our initial listening socket? It is still there, still
// listening. If we wanted to, we could call accept on it again. But remember
// accept will block until there is a second client connecting. And while your
// server is blocked, it can't be interacting with the first client.
// So to do this right, you'll need to combine some of the other things you've
// learned (like either the fork or select system calls) to be able to handle multiple
// clients simultaneously.

// So let's just stick with interactions between the server and one client for now.

// Notice that the type of the socket is int.
// You may have forgotten, but when we first talked about sockets, we said that
// this integer was an index in the file-descriptor table. We sometimes refer
// to the socket as the socket descriptor.
// The neat thing about the socket interface is that once we have the stream
// socket set up between the server and client, we use this socket descriptor
// just like we use a file-descriptor. So we can use all the things we know
// already about the read() and write() system calls.

// Let's make the server write to the client first. We will send 7 characters.
// Notice that \r and \n are each one character, so 5 characters for hello plus
// 2 additional whitespace characters is 7. We aren't explicitly sending the
// null character at the end of the string in this example.

// Why \r and \n? This is called a "network newline." We needed to specify a specific order
// for the bytes in multi-byte numeric types so that machines that use different byte
// orders can work together. Similarly, we have to specify how to identify a newline so
// that machines that use different sequences of characters will render strings appropriately.

// In the client we will need to allocate memory to hold the values we will read.
// I allocated 10 bytes which is more than we will need.
// Now we pass read the socket descriptor, the address of the memory
// where the values should be stored, and the maximum number of bytes to read.

// I'd like to print out the value that we read, to demonstrate that it is working.
// But there is a tiny problem. Can you see what it is?
// printf expects buf to be a string -- which means that it needs a null
// terminator.

// Remember that we didn't write a null terminator to the socket.
// So let's explicitly add one after the bytes that we read into buf.

// Let's compile both programs

// and run them, starting with the server -- which blocks for now waiting for
// a connection.

// and we will make a connection.

// And notice that it worked. Notice that the printf statement printed the newline
// // as well -- since we sent that.

// // Let's write back to the server. We use the same socket and again specify the
// exact number of bytes to write.

// Again I'll declare enough memory for 10 characters -- this time calling it
// line to demonstrate that there is nothing special about the name buf. But
// this time, I'm reading 10 characters so using all the memory available.

// So when I null-terminate the string, I don't have room to add to the values that
// were read and instead have to replace the last one.

// This time we see the same results as before except now the client writes 10
// characters to the server, the server reads 10 and then prints 9 of them.

// One important note: I haven't shown any error checking on read and in this video
// I assumed that the whole 7 and then the whole 10 bytes would be read in a
// single read. That worked ok here, but in general that's not safe.
// Remember that we are talking about communication over the internet. Data is being
// wrapped up in IP packets and sent from machine to machine and then reassembled at
// the other end. The stream socket protocol guarantees that we have lossless
// transmission -- we will eventually get all the data that was sent, in the same
// order as it was sent. But it does not guarantee that just because a bunch of data
// was written with a single write statement, that it will all arrive at the same
// time. As a result, there's no guarantee that it will all be available for the same
// read statement.

// In real programs, you will need to make use of the fact that the return value
// from read reports how many bytes were successfully read. And then, if you
// haven't yet read all the bytes you were expecting, your program needs to call
// read again.

// It is especially important to check the return value of read because if the write
// end of the socket is closed, read will return 0.  This tells you that you have
// read all the data that will ever be sent through the socket, and you should not
// call read on the socket again.

// Suppose you do call read again. What happens? If the other end of the socket is closed,
// your read call will immediately return 0. If the other end of the socket is open, your 
// read will block -- and it might appear as if your program has an infinite loop when what's really happening is your read call is blocking forever waiting for data that will never arrive. 

// It's also a good idea to explicitly add a close when you're done with the socket.
// This will cause the connection to be terminated. When your program exits, the
// system will send a close on any sockets that you don't explicitly terminate, but
// it's good practice to clean up after yourself. It also avoids problems that occur
// when a program doesn't immediately terminate, which leaves the other partner in
// the conversation hanging.

// So that's it! We've set up a socket on two machines and used it to communicate.