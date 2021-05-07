#include <stdio.h>
#include <stdlib.h>
#include "knn.h"

/**
 * Compilation command
 *    gcc -Wall -std=c99 -lm -o classifier classifier.c knn.c
 *
 * Decompress dataset into individual images.  The files in lists expect
 * the directory that contains the classifier executable also contains the
 * datasets directory.
 *    tar xvzf datasets.tgz
 *
 * Running quick test with 1k training and 1k testing images, K = 1:
 *    ./classifier 1 lists/training_1k.txt lists/testing_1k.txt
 *
 * Running full evaluation with all images, K = 7: (Will take a while)
 *    ./classifier 7 lists/training_full.txt lists/testing_full.txt
 */

/*****************************************************************************/
/* Do not add anything outside the main function here. Any core logic other  */
/* than what is described below should go into `knn.c`. You've been warned!  */
/*****************************************************************************/

/**
 * main() takes in 3 command line arguments:
 *    - K : The K value for K nearest neighbours
 *    - training_list: Name of a file with paths to a set of training images
 *    - testing_list:  Name of a file with paths to a set of testing images
 *
 * You need to do the following:
 *    - Parse the command line arguments, call `load_dataset()` appropriately.
 *    - For each test image, call `knn_predict()` and compare with real label
 *    - Use the printf statements provided to print the result of knn_predict
 *      (Do not print any other output)
 */

/* To store the data from the training and test datasets, we are using a 
 * series of arrays.  Generally speaking, this is a poor design decision
 * because global variables like this are frowned upon, because we are 
 * allocating more memory than necessary, and the code would be more 
 * readable if we could collect data that belonged together into objects
 * (structs).  It is also inflexible, because we cannot handle different 
 * image sizes.
 * 
 * However, we are using this approach because we have not yet covered
 * structs or dynamic memory allocation.
 */

/* training_dataset - a 2D array where each row contains the pixels for a 
 *                    training image
 * training_labels - an array that contains the labels for each image in
 *                   training_data set indicating which digit the image
 *                   represents
 * test_dataset - a 2D array where each row contains the pixels for an image
 *                that we want to classify
 * test_labels - an array containing the labels for the test images so we
 *               can tell whether the prediction was correct
 */
unsigned char training_dataset[MAX_SIZE][NUM_PIXELS];
unsigned char training_labels[MAX_SIZE];
unsigned char test_dataset[MAX_SIZE][NUM_PIXELS];
unsigned char test_labels[MAX_SIZE];


int main(int argc, char *argv[]) {  
    if (argc != 4) {
        fprintf(stderr, "Usage: %s K training_list test_images\n", argv[0]);
        exit(1);
    }
    char *training_file_list = argv[2];
    char *test_file_list = argv[3];
    int K = strtod(argv[1], NULL);

    int num_training_files = 0;
    int num_test_files = 0;
    int num_correct = 0;

    printf("Loading training data...\n");

    // TODO: (use load_dataset)
    num_training_files =  load_dataset(training_file_list, training_dataset, training_labels);

    printf("Loading testing data...\n");

    // TODO: (use load_dataset)
    num_test_files = load_dataset(test_file_list, test_dataset, test_labels);

    /* TODO: for each image in the test image dataset, call knn_predict
     * to make a prediction for what digit is represented.  If the
     * prediction matches the test image label, then increment the number
     * of correct predictions.  Hint: the return value of load_dataset tells
     * you how many images of each type you have.
     */
    for(int i = 0; i < MAX_SIZE; i++){
        int j = knn_predict(test_dataset[i],K, training_dataset, training_labels,num_training_files);

        int j = knn_predict(test_dataset[i], K, training_dataset, training_labels,num_training_files);
        if(j == test_labels[i]){
            num_correct += 1;
        }
    }


    // Print out answer
    printf("Number of correct predictions: %d\n", num_correct);
    printf("Accuracy: %.2f%%\n", 100.0*(double)num_correct/num_test_files);

    return 0;
}
