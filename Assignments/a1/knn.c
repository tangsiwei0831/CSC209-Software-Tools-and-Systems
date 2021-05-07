#include <stdio.h>
#include <math.h>    // Need this for sqrt()
#include <stdlib.h>
#include <string.h>

#include "knn.h"
#define MAX_DIST 255 * 28
#define LABEL_NUM 10
/* Print the image to standard output in the pgmformat.  
 * (Use diff -w to compare the printed output to the original image)
 */
void print_image(unsigned char *image) {
    printf("P2\n %d %d\n 255\n", WIDTH, HEIGHT);
    for (int i = 0; i < NUM_PIXELS; i++) {
        printf("%d ", image[i]);
        if ((i + 1) % WIDTH == 0) {
            printf("\n");
        }
    }
}

/* Return the label from an image filename.
 * The image filenames all have the same format:
 *    <image number>-<label>.pgm
 * The label in the file name is the digit that the image represents
 */
unsigned char get_label(char *filename) {
    // Find the dash in the filename
    char *dash_char = strstr(filename, "-");
    // Convert the number after the dash into a integer
    return (char) atoi(dash_char + 1);
}

/* ******************************************************************
 * Complete the remaining functions below
 * ******************************************************************/


/* Read a pgm image from filename, storing its pixels
 * in the array img.
 * It is recommended to use fscanf to read the values from the file. First, 
 * consume the header information.  You should declare two temporary variables
 * to hold the width and height fields. This allows you to use the format string
 * "P2 %d %d 255 "
 * 
 * To read in each pixel value, use the format string "%hhu " which will store
 * the number it reads in an an unsigned character.
 * (Note that the img array is a 1D array of length WIDTH*HEIGHT.)
 */
void load_image(char *filename, unsigned char *img) {
    // Open corresponding image file, read in header (which we will discard)
    FILE *f2 = fopen(filename, "r");
    if (f2 == NULL) {
        perror("fopen");
        exit(1);
    }
	//TODO
    int width, height;
    unsigned char c;
    fscanf(f2, "P2 %d %d 255 ", &width, &height);
    for(int i = 0; i < NUM_PIXELS; i++){
        fscanf(f2, "%hhu ", &c);
        img[i] = c;
    }
    // for(int i = 0; i < width * height; i++){
    //     printf("%hhu", img[i]);
    // }
    fclose(f2);
}


/**
 * Load a full dataset into a 2D array called dataset.
 *
 * The image files to load are given in filename where
 * each image file name is on a separate line.
 * 
 * For each image i:
 *  - read the pixels into row i (using load_image)
 *  - set the image label in labels[i] (using get_label)
 * 
 * Return number of images read.
 */
int load_dataset(char *filename, 
                 unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                 unsigned char *labels) {
    // We expect the file to hold up to MAX_SIZE number of file names
    FILE *f1 = fopen(filename, "r");
    if (f1 == NULL) {
        perror("fopen");
        exit(1);
    }

   //TODO
    char pgm_name[MAX_NAME];
    int i = 0;
    while(fscanf(f1, "%s\n", pgm_name) == 1){
        labels[i] = get_label(pgm_name);
        load_image(pgm_name, dataset[i]);
        i = i + 1;
    }
    // for(int i = 0; i < NUM_PIXELS; i++){
    //     printf("%hhu", dataset[0][i]);
    // }
    fclose(f1);
    return i;
}

/** 
 * Return the euclidean distance between the image pixels in the image
 * a and b.  (See handout for the euclidean distance function)
 */
double distance(unsigned char *a, unsigned char *b) {

    // TODO
    double sum, diff, pow = 0.0;
    for(int i = 0; i < NUM_PIXELS; i++){
        diff = a[i] - b[i];
        pow = diff * diff;
        sum += pow;
    };
    return sqrt(sum);
}

int find_rank(double *candidate_dists, int K, double dist) {
    for (int i = K - 1; i >= 0; i--) {
        if (dist >= candidate_dists[i]) {
            return i + 1;
        }
    }
    // if all the distances of current candiate images to input are greater.
    return 0;
}
/**
 * Return the most frequent label of the K most similar images to "input"
 * in the dataset
 * Parameters:
 *  - input - an array of NUM_PIXELS unsigned chars containing the image to test
 *  - K - an int that determines how many training images are in the
 *        K-most-similar set
 *  - dataset - a 2D array containing the set of training images.
 *  - labels - the array of labels that correspond to the training dataset
 *  - training_size - the number of images that are actually stored in dataset
 *
 * Steps
 *   (1) Find the K images in dataset that have the smallest distance to input
 *         When evaluating an image to decide whether it belongs in the set of
 *         K closest images, it will only replace an image in the set if its
 *         distance to the test image is strictly less than all of the images in
 *         the current K closest images.
 *   (2) Count the frequencies of the labels in the K images
 *   (3) Return the most frequent label of these K images
 *         In the case of a tie, return the smallest value digit.
 */

int knn_predict(unsigned char *input, int K,
                unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                unsigned char *labels,
                int training_size) {
    // store K clostest image ids in ascending order of distance to input
    int candidate_ids[K];
    // store K clostest image distances in ascending order of distance to input
    double candidate_dists[K];
    // initialization
    for (int i = 0; i < K; i++) {
        candidate_ids[i] = -1;
        candidate_dists[i] = MAX_DIST + 1;
    }
    // maintain K-most-similar set while traversing the input images
    for (int i = 0; i < training_size; i++) {
        double dist = distance(input, dataset[i]);
        int rank = find_rank(candidate_dists, K, dist);
        if (rank < K) {
            // current image should insert into the K-most-similar set
            for (int j = K - 1; j > rank; j--) {
                candidate_ids[j] = candidate_ids[j - 1];
                candidate_dists[j] = candidate_dists[j - 1];
            }
            candidate_ids[rank] = i;
            candidate_dists[rank] = dist;
        }
    }
    // couting all labels in K-most-similar set
    int label_counters[LABEL_NUM];
    for (int i = 0; i < LABEL_NUM; i++) {
        label_counters[i] = 0;
    }
    for (int i = 0; i < K; i++) {
      if (candidate_ids[i] != -1) {
        // this is a valid candidate
        int label_digit = labels[candidate_ids[i]];
        label_counters[label_digit]++;
      }
    }
    int label_count = -1, label_ans = -1;
    for (int i = 0; i < LABEL_NUM; i++) {
      if (label_counters[i] > label_count) {
        label_count = label_counters[i];
        label_ans = i;
      }
    }
    return label_ans;
}