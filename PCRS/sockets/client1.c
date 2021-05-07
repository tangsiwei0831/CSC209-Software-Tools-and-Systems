#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    // create socket
    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc == -1) {
        perror("client: socket");
        exit(1);
    }

    //initialize server address    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(54321);  
    memset(&server.sin_zero, 0, 8);
    
    struct addrinfo *ai;
    char * hostname = "teach.cs.toronto.edu";

    /* this call declares memory and populates ailist */
    getaddrinfo(hostname, NULL, NULL, &ai);
    /* we only make use of the first element in the list */
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;

    // free the memory that was allocated by getaddrinfo for this list
    freeaddrinfo(ai);

    int ret = connect(soc, (struct sockaddr *)&server, sizeof(struct sockaddr_in));

    printf("Connect returned %d\n", ret);
    return 0;
}

// In this series of videos we are examining one method for setting up client and server communication using
// C sockets.

// In the last video, we looked at the server-side system calls which bound a socket to an address and set it
// to listen for connections. In this video, we'll look at how a server accepts connections from a client.

// The accept system call takes three parameters, the first of which is the listening socket that we
// have been setting up and configuring.
// So let's put that in.

// The second parameter address is of type pointer to struct sockaddr. accept uses this parameter to communicate back to the caller the
// address of the client -- the machine that is attempting to connect. Let me say that again:
// When accept returns, the second parameter will point to a struct that holds the client's address information.

// So first notice that I said, WHEN accept returns. Like some other IO calls such as read, accept is a BLOCKING
// system call. It waits until a connection is established. So if you call accept and no client is attempting to
// connect, accept will not return immediately. It will block.

// accept can return if there was an error, so we need to check the return value.

// The return value is -1 when accept fails, and we can do the conventional error handling with
// perror.

// But on success, the value isn't simply 0. It's an integer representing a new socket which we will use to communicate
// with the client. We'll assign that value to a variable, so we can use it later, in the next video.

// Since the return value is being used to return a new socket, it can't be used to give us
// the address of the client -- and we might want that information. That's what the second parameter is
// used for. Before we call accept, we have to allocate memory for the struct, and then we pass a pointer
// to that struct into the accept call. accept uses the pointer to access the struct and set its fields.

// There's one more confusing catch. Remember how in the bind call the function's type used a generic struct
// but the actual call used a specific struct for a particular address family? We'll do the same thing here.

// We need a struct of type sockaddr_in  just like we did before. I'll call it client_addr
// because it is going to hold the address of the machine on the *other* end of the
// connection.

// and the only field that we need to set in this struct is the sin_family.

// And just like we did with bind, we have to pass the address of client_addr and cast it to the required type.

// Remember that bind had a third parameter that was the length of the address? We have almost the same
// thing here, except that it is a pointer to memory that holds the address length. That way accept
// can change the value.

// We set the length to the size of our address.

// and pass in the address of this value. Now that we have the parameters set for the accept call, we can talk about what it does.

// I'll compile with the debugging flag on.

// and then I'll run the resulting executable with gdb.

// Using l for list, I can see that line 44 is where I will call accept

// Let's set a breakpoint there, so my program will stop running just *before* it starts executing that line.

// And I'll start running it.

// and it breaks at line 44.

// when I press n (for next), line 44 begins execution but it just waits. The accept call doesn't return.
// It is blocking -- waiting for a connection on the listening socket.

// So now we need to go write a client program that will connect.

// Just like we did in the server, we need to create a stream socket that will use TCP to communicate
// over the internet.

// And next we use the connect system call to initiate a connection over this socket
// to the server.

// The first parameter is the socket that we just created.
// That's easy.

// The second parameter is the address of the socket on the server to which we want
// to connect. We have to know this address in order to write our program.

// Just like we did with
// bind in the previous video, we use a struct of type sockaddr_in and we set the
// field for sin_family to AF_INET.

// and we zero out the padding.

// Now, when you
// want to go order a latte at your local coffee shop, you have to know the address in order
// to find the server. The same is true here. You need the machine address and the port number.
// The port number that we used for the server was 54321 -- easy to remember. Again we have to convert
// it to network byte order.

// But what is the machine address?
// What you actually need for this field is the IP address. But we humans don't refer to machines by their IP addresses. We refer to them by their names.

// We will use a system call named getaddrinfo to look up the internet address of a machine based
// on its name.

// This is quite a powerful system call with lots of functionality to handle many variations
// in names and different protocols. We will show the simplest possible use case here and when you need
// more of the options you can learn more through a good systems programming book or online documentation.

// We will completely ignore the second and third parameters setting them to NULL.

// The first parameter is a string which is the name of the host machine.

// I'm going to fill in the name of the machine on which I'm running my server.

// This last parameter is the address of a pointer to a linked-list of structs.
// There might well be more than one address that satisfies your request for address information.
// Each element in the list is information about one of those valid addresses.
// The getaddrinfo() system call expects you to create a pointer and pass the address
// of the pointer. Then it allocates the memory for the linked-list of address information
// and sets your pointer to point to the list.

// I'll declare the pointer here.

// and pass its address as the last parameter to getaddrinfo().

// The system call allocated memory for the linked list on the heap and provides a
// function we call to free that memory when we are finished.

// But before we free the memory, we need to use the address information to set
// the server address.
// For our simple program, we will look at only the first address information struct
// from the linked list. That's the one pointed at directly by result. It
// has a field ai_addr that holds the information we need.
// But it's type is sockaddr (the generic address type that we have seen
// before), so we cast it to pointer to struct sockaddr_in (the type for internet addresses)
// And then from that sockaddr_in, we can look at only the sin_addr field, and
// assign that to the sin_addr field of the struct we are setting up for our
// connect call.

// So now we have a struct sockaddr_in that holds the server information
// and as we did before (for the bind system call), we need to cast it to
// type sockaddr needed for the second parameter in the connect call.

// And provide its length.

// And let's just print that returned value to confirm that the connection worked.
// In your final code you should of course use this value to check for errors and call perror as necessary.

// We'll compile our code.

// And we see that it runs successfully.

// And notice here that the server is no longer blocking on the accept call.
// It accepted the connection. And since that accept was the last line in the
// server program, it also finished running.

// Let's quit from executing the server. 

// Now that we don't have a server running, let's see what happens if I run
// the client again.

// We see that this time connect returns a -1. Connect was unsuccessful 
// there was nothing waiting at that port. Let's try again, with a
// slightly different scenario.

// We'll run the server again. And this time we've stopped right before the listen call. socket and bind have run.
// So there is a socket at this addresses, but the server isn't listening yet for connections.

// Again the client runs and the connect call returns a -1. Even though the server is running, it hasn't yet 
// set up the queue to listen for connections. 

// In this video, we successfully created a stream socket connection between a server and
// a client, and then we explored the blocking behaviour of accept. In the next video, we'll use that connection to communicate.
