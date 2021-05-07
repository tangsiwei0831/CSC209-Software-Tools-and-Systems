#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifndef PORT
  #define PORT 30000
#endif
#define MAX_BACKLOG 5
#define MAX_CONNECTIONS 20
#define BUF_SIZE 128
#define MAX_NAME 56

int verbose = 0;

struct user {
    int sock_fd;
    char name[MAX_NAME];
	int bid;
};

typedef struct {
    char *item;
    int highest_bid;  // value of the highest bid so far
    int client;       // index into the users array of the top bidder
} Auction;


/*
 * Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct user *users) {
    int user_index = 0;
    while (user_index < MAX_CONNECTIONS && users[user_index].sock_fd != -1) {
        user_index++;
    }

    if (user_index == MAX_CONNECTIONS) {
        fprintf(stderr, "server: max concurrent connections\n");
        return -1;
    }

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    users[user_index].sock_fd = client_fd;
    users[user_index].name[0] = '\0';
    return client_fd;
}

/* Remove \r\n from str if the characters are at the end of the string.
 * Defensively assuming that \r could be the last or second last character.
 */
void strip_newline(char *str) {
    if(str[strlen(str) - 1 ] == '\n' || str[strlen(str) - 1] == '\r') {
        if(str[strlen(str) - 2] == '\r') {
            str[strlen(str) - 2 ] = '\0';
        } else {
            str[strlen(str) - 1] = '\0';
        }
    }
}


/*
 * Read a name from a client and store in users.
 * Return the fd if it has been closed or 0 otherwise.
 */
int read_name(int client_index, struct user *users) {
    int fd = users[client_index].sock_fd;

    /* Note: This is not the best way to do this.  We are counting
     * on the client not to send more than BUF_SIZE bytes for the
     * name.
     */
   	int num_read = read(fd, users[client_index].name, MAX_NAME);
    if(num_read == 0) {
        users[client_index].sock_fd = -1;
        return fd;
    }
   	users[client_index].name[num_read] = '\0';
    strip_newline(users[client_index].name);

    if(verbose){
        fprintf(stderr, "[%d] Name: %s\n", fd, users[client_index].name);
    }

/*
    if (num_read == 0 || write(fd, buf, strlen(buf)) != strlen(buf)) {
        users[client_index].sock_fd = -1;
        return fd;
    }
*/
	
    return 0;
}

/* Read a bid from a client and store it in bid.
 * If the client does not send a number, bid will be set to -1
 * Return fd if the socket is closed, or 0 otherwise.
 */
int read_bid(int client_index, struct user *users, int *bid) {
    int fd = users[client_index].sock_fd;
    char buf[BUF_SIZE];
    char *endptr;
    int num_read = read(fd, buf, BUF_SIZE);
    if(num_read == 0) {
        return fd;
    }
   	buf[num_read] = '\0';

    if(verbose){
        fprintf(stderr, "[%d] bid: %s", fd, buf);
    }

    // Check if the client sent a valid number
    // (We are not checking for a good bid here.)
    errno = 0;
    *bid = strtol(buf, &endptr, 10);
    if(errno != 0 || endptr == buf) {
        *bid = -1;
    }

    return 0;
}

void broadcast(struct user *users, char *msg, int size) {
    for(int i = 0; i < MAX_CONNECTIONS; i++) {
        if(users[i].sock_fd != -1) {
            if(write(users[i].sock_fd, msg, size) == -1) {
                // Design flaw: can't remove this socket from select set
                close(users[i].sock_fd);
                users[i].sock_fd = -1;
            }
        }
    }
}


int prep_bid(char *buf, Auction *a, struct timeval *t) {
    // send item, current bid, time left in seconds

    sprintf(buf, "%s %d %ld", a->item, a->highest_bid, t->tv_sec);


    return 0;
}

/* Update auction if new_bid is higher than current bid.  
 * Write to the client who made the bid if it is lower
 * Broadcast to all clients if the bid is higher
 */
int update_bids(int client_index, struct user *users, 
                 int new_bid, Auction *auction, struct timeval *t) {
    char buf[BUF_SIZE];
    

    if(new_bid > auction->highest_bid) {
        auction->highest_bid = new_bid;
        auction->client = client_index;

        prep_bid(buf, auction, t);
        if(verbose) {
            fprintf(stderr, "[%d] Sending to %d:\n    %s\n", 
                    getpid(), users[client_index].sock_fd, buf);
        }
        
        broadcast(users, buf, strlen(buf) + 1);

    } else {
        fprintf(stderr, "Client %d sent bid that was too low.  Ignored\n",
                         client_index);
    }
    return 0;
}

int main(int argc, char **argv) {

    Auction auction;
    int opt;
    int port = PORT;
    struct timeval timeout;
    struct timeval *time_ptr = NULL;
    int minutes = 0;
    while((opt = getopt(argc, argv, "vt:p:")) != -1) {
        switch(opt) {
            case 'v':
                verbose = 1;
                break;
            case 't':
                minutes = atoi(optarg);
                timeout.tv_sec = minutes * 60;
                timeout.tv_usec = 0;
                time_ptr = &timeout;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: auction_server [-v] [-t timeout] [-p port] item\n");
                exit(1);
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(1);
    }

    auction.item = argv[optind];
    auction.client = -1;
    auction.highest_bid = -1;

    struct user users[MAX_CONNECTIONS];
    for (int index = 0; index < MAX_CONNECTIONS; index++) {
        users[index].sock_fd = -1;
        users[index].name[0] = '\0';
    }

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    // This sets an option on the socket so that its port can be reused right
    // away. Since you are likely to run, stop, edit, compile and rerun your
    // server fairly quickly, this will mean you can reuse the same port.
    int on = 1;
    int status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if (status == -1) {
        perror("setsockopt -- REUSEADDR");
    }

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    if(verbose) {
        fprintf(stderr, "[%d] Ready to accept connections on %d\n", 
                getpid(), port);
    }

    // The client accept - message accept loop. First, we prepare to listen 
	// to multiple file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        // select updates the fd_set it receives, so we always use a copy 
		// and retain the original.
        fd_set listen_fds = all_fds;
        int nready;
        if ((nready = select(max_fd + 1, &listen_fds, NULL, NULL, time_ptr)) == -1) {
            perror("server: select");
            exit(1);
        }
        if(nready == 0){
            char buf[BUF_SIZE];
            sprintf(buf, "Auction closed: %s wins with a bid of %d\r\n", 
                    users[auction.client].name, auction.highest_bid);
            printf("%s", buf);
            broadcast(users, buf, BUF_SIZE);
            exit(0);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, users);
            if(client_fd != -1) {
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }
                FD_SET(client_fd, &all_fds);
                if(verbose) {
                    fprintf(stderr, "[%d] Accepted connection on %d\n", 
                            getpid(), client_fd);
                }
            }
        }

        // Next, check the clients.
        for (int index = 0; index < MAX_CONNECTIONS; index++) {
            if (users[index].sock_fd > -1 && FD_ISSET(users[index].sock_fd, &listen_fds)) {
                int client_closed = 0;
                int new_bid = 0;
                if(users[index].name[0] == '\0') {
                    client_closed = read_name(index, users);
                    if(client_closed == 0){
                        char buf[BUF_SIZE];
                        prep_bid(buf, &auction, time_ptr);
                        if(verbose) {
                            fprintf(stderr, "[%d] Sending to %d:\n    %s\n", 
                                    getpid(), users[index].sock_fd, buf);
                        }           
                        if(write(users[index].sock_fd, buf, strlen(buf) + 1) == -1) {
                            fprintf(stderr, "Write to %d failed\n", sock_fd);   
                            close(sock_fd);
                        }

                    }

                } else {  // read a bid
                    client_closed = read_bid(index, users, &new_bid);
                    if(client_closed == 0) {
                        update_bids(index, users, new_bid, &auction, time_ptr);
                    }
                }

                if (client_closed > 0) {
                    FD_CLR(client_closed, &all_fds);
                    printf("Client %d disconnected\n", client_closed);
                } 
            }
        }
    }

    // Should never get here.
    return 1;
}
