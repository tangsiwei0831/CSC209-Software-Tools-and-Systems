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

#pragma once

/**
 * You will not be submitting this file, so do not change anything here
 * as it will not be reflected when the automarker is run. If you need any
 * extra structs / definitions, you can add them at the top of `dectree.c`
 * file, so they do not interfere with anything else.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *  For the recursive call with M images, we want to terminate recursion and 
 *  create a leaf node if the most frequent label in the set of M labels 
 *  makes up at least THRESHOLD_RATIO percent of the labels in the set
 *  In other words, we create a leaf node if:
 * 
 *      (freq of most common label) / M   >=  THRESHOLD_RATIO
 * 
 * NOTE: Do NOT hardcode this value in your solution, use this constant.
 */
#ifndef THRESHOLD_RATIO
#define THRESHOLD_RATIO 0.95
#endif

/* We will always be using square images */
#ifndef WIDTH
#define WIDTH 28
#endif

#ifndef NUM_PIXELS
#define NUM_PIXELS WIDTH * WIDTH
#endif

/**
 * The following structs represent the dataset. These structs differ
 * from the representation of the data set the following ways:
 * 1. We can now use a struct to represent an image. Note that we don't
 *    really need to store sx and sy because they will have the same
 *    values for all images in the data set.
 * 2. We can dynamically allocate memory for the number of images in 
 *    the file
 */

/* This struct stores the data for an image */
typedef struct {
    int sx;               // x resolution
    int sy;               // y resolution
    unsigned char *data;  // Array of `sx * sy` pixel color values [0-255]
} Image;

/* This struct stores the images / labels in the dataset */
typedef struct {
    int num_items;          // Number of images in the dataset
    Image *images;          // Array of `num_items` Image structs
    unsigned char *labels;  // Array of `num_items` labels [0-9]
} Dataset;


/* The following struct represents a node in the decision tree. */
typedef struct dt_node {
    int pixel;              // Which pixel to check in this node
    int classification;     // (Leaf nodes) Classification for this node
    struct dt_node *left;   // Left child   (color at `pixel` == 0)  
    struct dt_node *right;  // Right child  (color at `pixel` == 255)
} DTNode;


Dataset *load_dataset(const char *filename);

void get_most_frequent(Dataset *data, int M, int *indices, int *label, int *freq);
int find_best_split(Dataset *data, int M, int *indices);

DTNode *build_dec_tree(Dataset *data);
int dec_tree_classify(DTNode *root, Image *img);

void free_dataset(Dataset *data);
void free_dec_tree(DTNode *root);
