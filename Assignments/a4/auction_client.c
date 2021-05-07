#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define BUF_SIZE 128

#define MAX_AUCTIONS 5
#ifndef VERBOSE
#define VERBOSE 1
#endif

#define ADD 0
#define SHOW 1
#define BID 2
#define QUIT 3

/* Auction struct - this is different than the struct in the server program
 */
struct auction_data {
    int sock_fd;
    char item[BUF_SIZE];
    int current_bid;
};

/* Displays the command options available for the user.
 * The user will type these commands on stdin.
 */

void print_menu() {
    printf("The following operations are available:\n");
    printf("    show\n");
    printf("    add <server address> <port number>\n");
    printf("    bid <item index> <bid value>\n");
    printf("    quit\n");
}

/* Prompt the user for the next command 
 */
void print_prompt() {
    printf("Enter new command: ");
    fflush(stdout);
}


/* Unpack buf which contains the input entered by the user.
 * Return the command that is found as the first word in the line, or -1
 * for an invalid command.
 * If the command has arguments (add and bid), then copy these values to
 * arg1 and arg2.
 */
int parse_command(char *buf, int size, char *arg1, char *arg2) {
    int result = -1;
    char *ptr = NULL;
    if(strncmp(buf, "show", strlen("show")) == 0) {
        return SHOW;
    } else if(strncmp(buf, "quit", strlen("quit")) == 0) {
        return QUIT;
    } else if(strncmp(buf, "add", strlen("add")) == 0) {
        result = ADD;
    } else if(strncmp(buf, "bid", strlen("bid")) == 0) {
        result = BID;
    } 
    ptr = strtok(buf, " "); // first word in buf

    ptr = strtok(NULL, " "); // second word in buf
    if(ptr != NULL) {
        strncpy(arg1, ptr, BUF_SIZE);
    } else {
        return -1;
    }
    ptr = strtok(NULL, " "); // third word in buf
    if(ptr != NULL) {
        strncpy(arg2, ptr, BUF_SIZE);
        return result;
    } else {
        return -1;
    }
    return -1;
}

/* Connect to a server given a hostname and port number.
 * Return the socket for this server
 */
int add_server(char *hostname, int port) {
        // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }
    
    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    struct addrinfo *ai;
    
    /* this call declares memory and populates ailist */
    if(getaddrinfo(hostname, NULL, NULL, &ai) != 0) {
        close(sock_fd);
        return -1;
    }
    /* we only make use of the first element in the list */
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;

    // free the memory that was allocated by getaddrinfo for this list
    freeaddrinfo(ai);

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        return -1;
    }
    if (VERBOSE){
        fprintf(stderr, "\nDebug: New server connected on socket %d.  Awaiting item\n", sock_fd);
    }
    return sock_fd;
}
/* ========================= Add helper functions below ========================
 * Please add helper functions below to make it easier for the TAs to find the 
 * work that you have done.  Helper functions that you need to complete are also
 * given below.
 */

/* Print to standard output information about the auction
 */
void print_auctions(struct auction_data *a, int size) {
    printf("Current Auctions:\n");
    for (int index = 0; index < size; index++) {
        if (a[index].sock_fd != -1) {
            printf("(%d) %s bid = %d\n", index, a[index].item, a[index].current_bid);
        }
    }
}

/* Process the input that was sent from the auction server at a[index].
 * If it is the first message from the server, then copy the item name
 * to the item field.  (Note that an item cannot have a space character in it.)
 */
void update_auction(char *buf, int size, struct auction_data *a, int index) {
    char *ptr = NULL;
    char* endptr = NULL;
    int updated_bid = 0;
    long int seconds = 0;
    buf[size] = '\0';
    // extract first part as name
    ptr = strtok(buf, " ");
    if (!ptr) {
        fprintf(stderr, "ERROR no name\n");
        return;
    }
    if (a[index].item[0] == '\0') {
        strncpy(a[index].item, ptr, BUF_SIZE);
    }
    // extract second part as bid
    ptr = strtok(NULL, " ");
    if (!ptr) {
        fprintf(stderr, "ERROR no bid\n");
        return;
    }
    errno = 0;
    updated_bid = strtol(ptr, &endptr, 10);
    if (errno != 0 || endptr == ptr) {
        fprintf(stderr, "ERROR malformed bid: %s\n", ptr);
        return;
    } else {
        a[index].current_bid = updated_bid;
    }
    // extract third part as remaining seconds
    ptr = strtok(NULL, " ");
    if (!ptr) {
        fprintf(stderr, "ERROR no seconds\n");
        return;
    }
    errno = 0;
    seconds = strtol(ptr, &endptr, 10);
    if (errno != 0 || endptr == ptr) {
        fprintf(stderr, "ERROR malformed seconds: %s\n", ptr);
        return;
    }
    if (VERBOSE) {
        printf("\nDebug: New bid for %s [%d] is %d (%ld seconds left)\n", a[index].item, 
           index, a[index].current_bid, seconds);
    }
}

int main(void) {
    char name[BUF_SIZE];

    // Declare and initialize necessary variables
    struct auction_data a[MAX_AUCTIONS];
    for (int index = 0; index < MAX_AUCTIONS; index++) {
        a[index].sock_fd = -1;
        a[index].item[0] = '\0';
        a[index].current_bid = -1;
    }

    char buf[BUF_SIZE];
    char arg1[BUF_SIZE];
    char arg2[BUF_SIZE];

    // Get the user to provide a name.
    printf("Please enter a username: ");
    fflush(stdout);
    int num_read = read(STDIN_FILENO, name, BUF_SIZE - 1);
    if(num_read <= 0){
        fprintf(stderr, "ERROR: read from stdin failed\n");
        exit(1);
    }
    name[num_read] = '\0';
    print_menu();

    int max_fd = STDERR_FILENO;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(STDERR_FILENO, &all_fds);

    print_prompt();

    while(1) {
        // select updates the fd_set it receives, so we always use a copy 
		// and retain the original.
        fd_set listen_fds = all_fds;
        if (select(max_fd + 1, &listen_fds, NULL, NULL, NULL) == -1) {
            perror("client: select");
            exit(1);
        }
        
        // First, check the servers
        for (int index = 0; index < MAX_AUCTIONS; index++) {
            if (a[index].sock_fd > -1 && FD_ISSET(a[index].sock_fd, &listen_fds)) {
                num_read = read(a[index].sock_fd, buf, BUF_SIZE - 1);
                if (num_read <= 0){
                    fprintf(stderr, "ERROR: read from server %d failed\n", index);
                    continue;
                }
                buf[num_read] = '\0';
                if (strncmp(buf, "Auction closed:", strlen("Auction closed:")) == 0) {
                    if (VERBOSE) {
                        printf("\nDebug: Get message from server %d: %s\n", index, buf);
                        printf("\nDebug: Disconnected from server %d\n", index);
                    }
                    close(a[index].sock_fd);
                    FD_CLR(a[index].sock_fd, &all_fds);
                    a[index].sock_fd = -1;
                } else {
                    if (VERBOSE) {
                        printf("\nDebug: Get message from server %d: %s\n", index, buf);
                    }
                    update_auction(buf, num_read, a, index);
                }
            }
        }

        // Next, check the standard input
        if (FD_ISSET(STDERR_FILENO, &listen_fds)) {
            num_read = read(STDIN_FILENO, buf, BUF_SIZE - 1);
            if(num_read <= 0){
                fprintf(stderr, "ERROR: read from stdin failed\n");
                exit(1);
            }
            buf[num_read] = '\0';
            int result = parse_command(buf, num_read, arg1, arg2);
            if (result == SHOW) {
                print_auctions(a, MAX_AUCTIONS);
            } else if (result == ADD) {
                errno = 0;
                char *endptr;
                int port = strtol(arg2, &endptr, 10);
                if (errno != 0 || endptr == arg2) {
                    fprintf(stderr, "ERROR malformed port: %s\n", arg2);
                } else {
                    int server_index = 0;
                    while (server_index < MAX_AUCTIONS && a[server_index].sock_fd != -1) {
                        server_index++;
                    }
                    if (server_index == MAX_AUCTIONS) {
                        fprintf(stderr, "client: max concurrent connections\n");
                    } else {
                        int server_fd = add_server(arg1, port);
                        if (server_fd != -1) {
                            printf("Connected to server %s:%d\n", arg1, port);
                            if (server_fd > max_fd) {
                                max_fd = server_fd;
                            }
                            if (write(server_fd, name, strlen(name) + 1) == -1) {
                                fprintf(stderr, "Failed to send name to server %s:%d\n", arg1, port);
                                close(server_fd);
                            } else {
                                FD_SET(server_fd, &all_fds);
                                a[server_index].sock_fd = server_fd;
                                printf("Sent name to server %s:%d\n", arg1, port);
                            }
                        } else {
                            fprintf(stderr, "Failed to connect to server %s:%d\n", arg1, port);
                        }
                    }
                }
            } else if (result == BID) {
                errno = 0;
                char *endptr;
                int index = strtol(arg1, &endptr, 10);
                if (errno != 0 || endptr == arg1) {
                    fprintf(stderr, "ERROR malformed index: %s\n", arg1);
                } else {
                    if (index < 0 || index >= MAX_AUCTIONS) {
                        fprintf(stderr, "Invalid item index: %d\n", index);
                    } else {
                        if (a[index].sock_fd != -1) {
                            if (write(a[index].sock_fd, arg2, strlen(arg2) + 1) == -1) {
                                fprintf(stderr, "Failed to send bid to server %d\n", index);
                                close(a[index].sock_fd);
                                FD_CLR(a[index].sock_fd, &all_fds);
                                a[index].sock_fd = -1;
                            } else {
                                printf("Sent bid to server %d\n", index);
                            }
                        } else {
                            fprintf(stderr, "Client is not connected to server %d\n", index);
                        }
                    }
                }
            } else if (result == QUIT) {
                for (int index = 0; index < MAX_AUCTIONS; index++) {
                    if (a[index].sock_fd != -1) {
                        close(a[index].sock_fd);
                        FD_CLR(a[index].sock_fd, &all_fds);
                        a[index].sock_fd = -1;
                    }
                }
                exit(0);
            } else {
                fprintf(stderr, "ERROR invalid command\n");
            }
            print_prompt();
        }
    }

    return 0; // Shoud never get here
}
