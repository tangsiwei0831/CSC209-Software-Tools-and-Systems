#pragma once

/**
 * You should not add anything this file except the value for MAX_SIZE
 * You will not need any other types or data structures other than what
 * has already been defined.
 */


#define WIDTH 28  // image width
#define HEIGHT 28 // image height
#define NUM_PIXELS WIDTH * HEIGHT

/* The maximum size of a training set or testing set of images
 * The number of images loaded into the training_dataset or test_dataset
 * may be smaller than the maximum, but this allows us to create the global
 * array at compile time.
 */
#ifndef MAX_SIZE
#define MAX_SIZE 60000
#endif

/* You will be reading names of files from the training file list and test 
 * file list.  Use MAX_NAME to declare the array to hold the input.  You can
 * assume that no name will be longer than 128 bytes.
 */
#define MAX_NAME 128

/* These functions are defined in knn.c  Their prototypes are included
 * here so that we don't have to write out the definitions for these
 * functions in the files that use them (e.g. classifier.c)
 */
void load_image(char *filename, unsigned char *img);

void print_image(unsigned char *img);

int load_dataset(char *filename,
                 unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                 unsigned char *labels);
double distance(unsigned char *a, unsigned char *b);

int knn_predict(unsigned char *input, int K,
                unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                unsigned char *labels, int training_size);
