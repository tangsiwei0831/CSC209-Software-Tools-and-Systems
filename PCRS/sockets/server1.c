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

    int return_value = accept(listen_soc, (struct sockaddr *)&client_addr, &client_len);
    return 0;
}

// In the last video we saw one way that we could set up an endpoint for communication between two processes
// by using the socket system call to create a stream socket. Now we will configure that socket
// to wait for connections at a specific address.

// So far we have created a socket like this. That sets up our stream socket, but doesn't set the address. We will
// use the bind system call for this.

// Notice that bind takes three parameters. The first is the socket that we want to configure -- the one
// we just created. So setting that one is easy.

// The second parameter (named address) is of type pointer to struct sockaddr.
// Pay attention here.

// pointer to struct sockaddr

// bind works for all the different address families. This type is generic. It is for all the families.
// But for our particular family AF_INET, we will set this  parameter by using a struct sockaddr_in.
// (in for "internet" not for in vs out.)

// Here is the definition of this struct.

// First, we create the struct we need.

// For the first field (sin_family), we use AF_INET to be consistent with our socket.

// The second field (sin_port) is where we set the port number. Port numbers range from
// 0 to just over 65 thousand, but the lowest 1024 are reserved for well-known services. For example, you
// might have heard of the program Telnet. This is a very old program that allows users to remotely access another
// machine. It runs on port 23. The next range of ports (from 1024 to 49,151) are called
// registered ports. If you use these ports for a service you wish to make public, you can register
// with IANA (the Internet Assigned Numbers Authority). The IANA also looks after assigning
// domain names at the highest level. If you are writing a server to run on your own machine, any port over 49151
// will be fine. If you are writing a server to run on a shared machine, then you don't want to set up a socket
// on a port that some other program is already using (say one of your classmate's.) In that case you'll have to work out a
// plan for who uses which port. 

// Let's suppose your instructor told you to use port 54321 for a course project.

// When I add the code to set that port, notice that I'm adding a extra call to the function htons.
// htons stands for host-to-network-short. 
// In memory, storing an integer takes more than one byte and different
// machines store the bytes that make up that integer in different orders. This is a big idea: when we write network
// code, we have to make sure that the two machines that are communicating are speaking the same language.
// They have to transmitting -- and expecting -- particular data in a specific format. We call these agreements
// "protocols," and they are essential when we communicate between different programs particularly over a network.

// So, in this case, we need to make sure that the integers we send are in the correct format. We use htons
// to convert the integer from the byte order of the host machine to what the protocol says is "network order." If the machine compiling and
// running this code already uses network byte order, then htons will do nothing. But if the very same code is
// compiled on a machine that doesn't use what the protocol specifies as network byte order, the function will flip the bytes around
// as needed.

// Notice that the field of struct sockaddr_in that holds the machine address is
// actually a struct itself.

// The only field of this inside struct that we need to set is s_addr

// and we can set it to the predefined constant INADDR_ANY. This configures the socket
// to accept connections from any of the addresses of the machine. But wait, didn't you
// say earlier that a machine had a single address? Not always. A machine can have multiple
// network interface cards and can be plugged into separate networks. It would have a different IP
// on each network. The machine also has
// an address for itself. The address 127.0.0.1 (called localhost) refers to the machine
// running the program. teach.cs.toronto.edu (the teaching machine here at the University of
// Toronto) has the IP address 128.100.31.200, and once I start my server, you could
// connect to it using that address. But if I run my client program on the same machine, I could
// also connect with 127.0.0.1.

// The last field sin_zero is extra padding. This makes the sockaddr_in struct the same length as the sockaddr
// struct. When we malloc the space for this struct, those bytes are not reset in any way. They hold the
// values that were there in memory before the malloc call.
// Since we are going to be sending these bytes over the internet, we don't want to introduce any potential security
// problems by exposing the previous contents to potential eves-droppers.

// So we use the function memset to set these 8 bytes to zero.

// Now that we have our addr struct initialized, we are ready to pass it as the second
// parameter to bind.
// But wait, bind is expecting a pointer to struct sockaddr not a struct sockaddr_in. So we need
// to do two things.

// First, we need to take the address of our struct. Now that's a pointer to struct sockaddr_in.

// Second, we need to cast this to struct sockaddr * to indicate that we know the types don't match
// precisely. This will keep the compiler happy.

// The last parameter to bind is the length of the address that we are passing.

// We can get this with by using the sizeof command on struct sockaddr_in. There is
// some disagreement about this historical reasons for *why* bind requires this size since
// we already padded out sockaddr_in to be the same length as sockaddr, but it is needed.
// So put it in.

// Finally, we are ready to call bind. And what about this return value? It is for error checking.

// Like most of our system calls, bind returns 0 on success and 1 on failure. Confirming the success of bind is
// particularly important; Suppose the port you picked is not available because it is already in use. Bind will fail
// and you wouldn't want your program to continue running without alerting you if this happened.

// So now we have a socket, bound to a particular port on a particular machine. Next, we need to
// tell the machine to start looking for connections.

// The system call we need to accomplish this is called listen.
// This system call is comparatively simple.

// The first parameter is the same socket we are setting up.

// The return value is for error checking. We'll put this call into the conditional of an if statement
// so we can raise an error if necessary.

// The second parameter backlog needs a little more explanation. Since we are using a connection-oriented
// socket, there is some work involved in setting up the connection between the server and the client.
// Between the time one client starts to connect and finally gets set up, there might be another client
// that attempts to connect. The listen system call keep some number of these pending connections (requests
// to whom it has replied trying to establish the connection but hasn't heard back from). listen sets up
// the data-structure needed to store these partial connections and backlog is the maximum number that
// it can hold. It isn't the maximum number of connections to the server in the end, just the maximum number of
// partially completed connections that can be held at any one time.

// We'll just set this to 5. That should be plenty for our purposes.

// The last system call that our server needs in order to establish a connection with a client is accept. But there
// is enough to talk about with accept that we'll do it in the next video.