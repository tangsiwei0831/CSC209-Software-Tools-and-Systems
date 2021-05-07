#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      
#include <sys/types.h>  
#include <sys/wait.h>  
#include <string.h>
#include <math.h>
#include "knn.h"

/*****************************************************************************/
/* Do not add anything outside the main function here. Any core logic other  */
/*   than what is described below should go in `knn.c`. You've been warned!  */
/*****************************************************************************/

/**
 * main() takes in the following command line arguments.
 *   -K <num>:  K value for kNN (default is 1)
 *   -d <distance metric>: a string for the distance function to use
 *          euclidean or cosine (or initial substring such as "eucl", or "cos")
 *   -p <num_procs>: The number of processes to use to test images
 *   -v : If this argument is provided, then print additional debugging information
 *        (You are welcome to add print statements that only print with the verbose
 *         option.  We will not be running tests with -v )
 *   training_data: A binary file containing training image / label data
 *   testing_data: A binary file containing testing image / label data
 *   (Note that the first three "option" arguments (-K <num>, -d <distance metric>,
 *   and -p <num_procs>) may appear in any order, but the two dataset files must
 *   be the last two arguments.
 * 
 * You need to do the following:
 *   - Parse the command line arguments, call `load_dataset()` appropriately.
 *   - Create the pipes to communicate to and from children
 *   - Fork and create children, close ends of pipes as needed
 *   - All child processes should call `child_handler()`, and exit after.
 *   - Parent distributes the test dataset among children by writing:
 *        (1) start_idx: The index of the image the child should start at
 *        (2)    N:      Number of images to process (starting at start_idx)
 *     Each child should get at most N = ceil(test_set_size / num_procs) images
 *      (The last child might get fewer if the numbers don't divide perfectly)
 *   - Parent waits for children to exit, reads results through pipes and keeps
 *      the total sum.
 *   - Print out (only) one integer to stdout representing the number of test 
 *      images that were correctly classified by all children.
 *   - Free all the data allocated and exit.
 *   - Handle all relevant errors, exiting as appropriate and printing error message to stderr
 */
void usage(char *name) {
    fprintf(stderr, "Usage: %s -v -K <num> -d <distance metric> -p <num_procs> training_list testing_list\n", name);
}

int main(int argc, char *argv[]) {

    int opt;
    int K = 1;             // default value for K
    char *dist_metric = "euclidean"; // default distant metric
    int num_procs = 1;     // default number of children to create
    int verbose = 0;       // if verbose is 1, print extra debugging statements
    int total_correct = 0; // Number of correct predictions

    while((opt = getopt(argc, argv, "vK:d:p:")) != -1) {
        switch(opt) {
        case 'v':
            verbose = 1;
            break;
        case 'K':
            K = atoi(optarg);
            break;
        case 'd':
            dist_metric = optarg;
            break;
        case 'p':
            num_procs = atoi(optarg);
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    if(optind >= argc) {
        fprintf(stderr, "Expecting training images file and test images file\n");
        exit(1);
    } 

    char *training_file = argv[optind];
    optind++;
    char *testing_file = argv[optind];

    double (*fptr)(Image*, Image*) = NULL;
    if (strncmp(dist_metric, "euclidean", strlen(dist_metric)) == 0) {
        fptr = distance_euclidean;
    } else if (strncmp(dist_metric, "cosine", strlen(dist_metric)) == 0) {
        fptr = distance_cosine;
    } else {
        fprintf(stderr, "Distance metric %s does not exist\n", dist_metric);
        exit(1);
    }

    // Load data sets
    if(verbose) {
        printf("- Loading datasets...\n");
    }
    
    Dataset *training = load_dataset(training_file);
    if ( training == NULL ) {
        fprintf(stderr, "The data set in %s could not be loaded\n", training_file);
        exit(1);
    }

    Dataset *testing = load_dataset(testing_file);
    if ( testing == NULL ) {
        fprintf(stderr, "The data set in %s could not be loaded\n", testing_file);
        exit(1);
    }

    if(verbose) {
        printf("- Creating children ...\n");
    }

    int pipe_pc[num_procs][2]; // pipes for unidirectional communication: parent -> child
    int pipe_cp[num_procs][2]; // pipes for unidirectional communication: child -> parent
    int chunk_size = ceil((float)testing->num_items / num_procs);
    for (int i = 0; i < num_procs; i++) {
        // Create the pipes and child processes who will then call child_handler
        if (pipe(pipe_pc[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        if (pipe(pipe_cp[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        int result = fork();
        if (result < 0) {
            perror("fork");
            exit(1);
        } else if (result == 0) {
            if (close(pipe_pc[i][1]) == -1) {
                perror("close writing end of pipe (parent -> child)");
                exit(1);
            }
            if (close(pipe_cp[i][0]) == -1) {
                perror("close reading end of pipe (child -> parent)");
                exit(1);
            }
            child_handler(training, testing, K, fptr, pipe_pc[i][0], pipe_cp[i][1]);
            if (close(pipe_pc[i][0]) == -1) {
                perror("close reading end of pipe (parent -> child)");
                exit(1);
            }
            if (close(pipe_cp[i][1]) == -1) {
                perror("close writing end of pipe (child -> parent)");
                exit(1);
            }
            exit(0);
        } else {
            if (verbose) {
                printf("- Created child %d\n", result);
            }
            if (close(pipe_pc[i][0]) == -1) {
                perror("close reading end of pipe (parent -> child)");
                exit(1);
            }
            if (close(pipe_cp[i][1]) == -1) {
                perror("close writing end of pipe (child -> parent)");
                exit(1);
            }
            // Distribute the work to the children by writing their starting index and
            // the number of test images to process to their write pipe
            int start_index = i * chunk_size;
            if (verbose) {
                printf("- Give child %d start_index %d and #test_images %d\n", result, start_index, chunk_size);
            }
            if (write(pipe_pc[i][1], &start_index, sizeof(int)) != sizeof(int)) {
                perror("write data to pipe (parent->child)");
                exit(1);
            }
            if (write(pipe_pc[i][1], &chunk_size, sizeof(int)) != sizeof(int)) {
                perror("write data to pipe (parent->child)");
                exit(1);
            }
            if (close(pipe_pc[i][1]) == -1) {
                perror("close writing end of pipe (parent -> child)");
                exit(1);
            }
        }
    }

    if(verbose) {
        printf("- Waiting for children...\n");
    }

    int status;
    int pid;
    int contribution;
    for (int i = 0; i < num_procs; i++) {
        // Wait for children to finish
        if ((pid = wait(&status)) == -1) {
            perror("wait");
            exit(1);
        }
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                // When the children have finised, read their results from their pipe
                if (read(pipe_cp[i][0], &contribution, sizeof(int)) != sizeof(int)) {
                    perror("reading from pipe from a child");
                    exit(1);
                }
                if (close(pipe_cp[i][0]) == -1) {
                    perror("close reading end of pipe (child -> parent)");
                    exit(1);
                }
                total_correct += contribution;
                if (verbose) {
                    printf("- Child %d correctly recognized %d images\n", pid, contribution);
                }
            } else {
                fprintf(stderr, "Child %d exited with status %d\n", pid, WEXITSTATUS(status));
                exit(1);
            }
        } else {
            fprintf(stderr, "Child %d exited with signal %d\n", pid, WTERMSIG(status));
            exit(1);
        }
    }

    if(verbose) {
        printf("Number of correct predictions: %d\n", total_correct);
    }

    // This is the only print statement that can occur outside the verbose check
    printf("%d\n", total_correct);

    // Clean up any memory, open files, or open pipes
    free_dataset(training);
    free_dataset(testing);

    return 0;
}
