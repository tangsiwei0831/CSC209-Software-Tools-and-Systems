/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC209H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Mustafa Quraish, Bianca Schroeder, Karen Reid
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2021 Karen Reid
 */

#include "dectree.h"

// Makefile included in starter:
//    To compile:               make
//    To decompress dataset:    make datasets
//
// Running decision tree generation / validation:
//    ./classifier datasets/training_data.bin datasets/testing_data.bin

/*****************************************************************************/
/* Do not add anything outside the main function here. Any core logic other  */
/* than what is described below should go in `dectree.c`. You've been warned!*/
/*****************************************************************************/

/**
 * main() takes in 2 command line arguments:
 *    - training_data: A binary file containing training image / label data
 *    - testing_data: A binary file containing testing image / label data
 *
 * You need to do the following:
 *    - Parse the command line arguments, call `load_dataset()` appropriately.
 *    - Call `make_dec_tree()` to build the decision tree with training data
 *    - For each test image, call `dec_tree_classify()` and compare the real 
 *        label with the predicted label
 *    - Print out (only) one integer to stdout representing the number of 
 *        test images that were correctly classified.
 *    - Free all the data dynamically allocated and exit.
 * 
 */
int main(int argc, char *argv[]) {
  if (argc != 3) {
        fprintf(stderr, "Usage: %s train_dataset test_dataset\n", argv[0]);
        return 1;
    }
    
    // Parse the command line
    int total_correct = 0;
    char* train_dataset_filename = argv[1];
    char* test_dataset_filename = argv[2];

    // Load training dataset
    Dataset* train_data = load_dataset(train_dataset_filename);
    if (!train_data) {
        fprintf(stderr, "Failed to load dataset %s\n", train_dataset_filename);
        return 1;
    }

    // Load test dataset
    Dataset* test_data = load_dataset(test_dataset_filename);
    if (!test_data) {
        fprintf(stderr, "Failed to load dataset %s\n", test_dataset_filename);
        free_dataset(train_data);
        return 1;
    }

    // Build decision tree
    DTNode* root = build_dec_tree(train_data);
    if (!root) {
        fprintf(stderr, "Failed to build decision tree\n");
        free_dataset(train_data);
        free_dataset(test_data);
        return 1;
    }

    // Predict labels based on decision tree
    int predicted_label = -1;
    for (int i = 0; i < test_data->num_items; i++) {
        predicted_label = dec_tree_classify(root, test_data->images + i);
        total_correct += (predicted_label == test_data->labels[i]);
    }

    // Print out answer
    printf("%d\n", total_correct);

    // Free all dynamically allocated memory
    free_dataset(train_data);
    free_dataset(test_data);
    free_dec_tree(root);

    return 0;
}
