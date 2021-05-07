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
#define THRESHOLD_DPETH 20

/**
 * Load the binary file, filename into a Dataset and return a pointer to 
 * the Dataset. The binary file format is as follows:
 *
 *     -   4 bytes : `N`: Number of images / labels in the file
 *     -   1 byte  : Image 1 label
 *     - NUM_PIXELS bytes : Image 1 data (WIDTHxWIDTH)
 *          ...
 *     -   1 byte  : Image N label
 *     - NUM_PIXELS bytes : Image N data (WIDTHxWIDTH)
 *
 * You can set the `sx` and `sy` values for all the images to WIDTH. 
 * Use the NUM_PIXELS and WIDTH constants defined in dectree.h
 */
Dataset *load_dataset(const char *filename) {
    // Open file
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return NULL;
    }
    // Allocate and initialize dataset
    Dataset* data = (Dataset*)malloc(sizeof(Dataset));
    if (!data) {
        fprintf(stderr, "Failed to allocate space for dataset.\n");
        fclose(f);
        return NULL;
    }
    data->num_items = 0;
    data->images = NULL;
    data->labels = NULL;
    // Read a number
    int ret = 0;
    ret = fread(&data->num_items, 4, 1, f);
    if (ret != 1) {
        fprintf(stderr, "Failed to read %s.\n", filename);
        free_dataset(data);
        fclose(f);
        return NULL;
    }
    // Allocate and initialize image array
    data->images = (Image*)malloc(data->num_items * sizeof(Image));
    if (!data->images) {
        fprintf(stderr, "Failed to allocate space for images.\n");
        free_dataset(data);
        fclose(f);
        return NULL;
    }
    for (int i = 0; i < data->num_items; i++) {
        data->images[i].data = NULL;
    }
    // Allocate data array
    for (int i = 0; i < data->num_items; i++) {
        data->images[i].data = (unsigned char*)malloc(NUM_PIXELS * sizeof(unsigned char));
        if (!data->images[i].data) {
            fprintf(stderr, "Failed to allocate space for images.\n");
            free_dataset(data);
            fclose(f);
            return NULL;
        }
        data->images[i].sx = WIDTH;
        data->images[i].sy = WIDTH; 
    }
    // Allocate label array
    data->labels = (unsigned char*)malloc(data->num_items * sizeof(unsigned char));
    if (!data->labels) {
        fprintf(stderr, "Failed to allocate space for image labels.\n");
        free_dataset(data);
        fclose(f);
        return NULL;
    }
    // Fill image data and label arrays with file content
    for (int i = 0; i < data->num_items; i++) {
        ret = fread(data->labels + i, 1, 1, f);
        if (ret != 1) {
            fprintf(stderr, "Failed to read %s.\n", filename);
            free_dataset(data);
            fclose(f);
            return NULL;
        }
        ret = fread(data->images[i].data, 1, NUM_PIXELS, f);
        if (ret != NUM_PIXELS) {
            fprintf(stderr, "Failed to read %s.\n", filename);
            free_dataset(data);
            fclose(f);
            return NULL;
        }
    }
    fclose(f);
    return data;
}

/**
 * Compute and return the Gini impurity of M images at a given pixel
 * The M images to analyze are identified by the indices array. The M
 * elements of the indices array are indices into data.
 * This is the objective function that you will use to identify the best 
 * pixel on which to split the dataset when building the decision tree.
 *
 * Note that the gini_impurity implemented here can evaluate to NAN 
 * (Not A Number) and will return that value. Your implementation of the 
 * decision trees should ensure that a pixel whose gini_impurity evaluates 
 * to NAN is not used to split the data.  (see find_best_split)
 * 
 * DO NOT CHANGE THIS FUNCTION; It is already implemented for you.
 */
double gini_impurity(Dataset *data, int M, int *indices, int pixel) {
    int a_freq[10] = {0}, a_count = 0;
    int b_freq[10] = {0}, b_count = 0;

    for (int i = 0; i < M; i++) {
        int img_idx = indices[i];

        // The pixels are always either 0 or 255, but using < 128 for generality.
        if (data->images[img_idx].data[pixel] < 128) {
            a_freq[data->labels[img_idx]]++;
            a_count++;
        } else {
            b_freq[data->labels[img_idx]]++;
            b_count++;
        }
    }

    double a_gini = 0, b_gini = 0;
    for (int i = 0; i < 10; i++) {
        double a_i = ((double)a_freq[i]) / ((double)a_count);
        double b_i = ((double)b_freq[i]) / ((double)b_count);
        a_gini += a_i * (1 - a_i);
        b_gini += b_i * (1 - b_i);
    }

    // Weighted average of gini impurity of children
    return (a_gini * a_count + b_gini * b_count) / M;
}

/**
 * Given a subset of M images and the array of their corresponding indices, 
 * find and use the last two parameters (label and freq) to store the most
 * frequent label in the set and its frequency.
 *
 * - The most frequent label (between 0 and 9) will be stored in `*label`
 * - The frequency of this label within the subset will be stored in `*freq`
 * 
 * If multiple labels have the same maximal frequency, return the smallest one.
 */
void get_most_frequent(Dataset *data, int M, int *indices, int *label, int *freq) {
    // TODO: Set the correct values and return
    int counters[10] = {0}, count = 0;
    for (int i = 0; i < M; i++) {
        counters[data->labels[indices[i]]]++;
    }
    for (int i = 0; i < 10; i++) {
        if (counters[i] > count) {
            *label = i;
            count = counters[i];
        }
    }
    *freq = count / M;
}

/**
 * Given a subset of M images as defined by their indices, find and return
 * the best pixel to split the data. The best pixel is the one which
 * has the minimum Gini impurity as computed by `gini_impurity()` and 
 * is not NAN. (See handout for more information)
 * 
 * The return value will be a number between 0-783 (inclusive), representing
 *  the pixel the M images should be split based on.
 * 
 * If multiple pixels have the same minimal Gini impurity, return the smallest.
 */
int find_best_split(Dataset *data, int M, int *indices) {
    // TODO: Return the correct pixel
    double min_gini_impurity = INFINITY, temp_gini_impurity = 0;
    int best_pixel = -1;
    for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {
        temp_gini_impurity = gini_impurity(data, M, indices, pixel);
        if (temp_gini_impurity < min_gini_impurity) {
            min_gini_impurity = temp_gini_impurity;
            best_pixel = pixel;
        }
    }
    return best_pixel;
}

/**
 * Create the Decision tree. In each recursive call, we consider the subset of the
 * dataset that correspond to the new node. To represent the subset, we pass 
 * an array of indices of these images in the subset of the dataset, along with 
 * its length M. Be careful to allocate this indices array for any recursive 
 * calls made, and free it when you no longer need the array. In this function,
 * you need to:
 *
 *    - Compute ratio of most frequent image in indices, do not split if the
 *      ration is greater than THRESHOLD_RATIO
 *    - Find the best pixel to split on using `find_best_split`
 *    - Split the data based on whether pixel is less than 128, allocate 
 *      arrays of indices of training images and populate them with the 
 *      subset of indices from M that correspond to which side of the split
 *      they are on
 *    - Allocate a new node, set the correct values and return
 *       - If it is a leaf node set `classification`, and both children = NULL.
 *       - Otherwise, set `pixel` and `left`/`right` nodes 
 *         (using build_subtree recursively). 
 */
DTNode *build_subtree(Dataset *data, int M, int *indices) {
    // TODO: Construct and return the tree
    static int depth = 0;
    // Increase the node depth
    depth++;
    // Allocate a new node
    DTNode *node = (DTNode*)malloc(sizeof(DTNode));
    if (!node) {
        fprintf(stderr, "Failed to allocate space for decision tree node.\n");
        depth--;
        return NULL;
    }
    // Check if we are building a leaf node
    int label = -1, freq = 0;
    get_most_frequent(data, M, indices, &label, &freq);
    if (freq >= THRESHOLD_RATIO || depth == THRESHOLD_DPETH) {
        node->pixel = -1;
        node->classification = label;
        node->left = NULL;
        node->right = NULL;
        depth--;
        return node;
    }
    // Build an internal node
    int pixel = find_best_split(data, M, indices);
    int *left_indices = (int*)malloc(M * sizeof(int));
    if (!left_indices) {
        fprintf(stderr, "Failed to allocate space for image indices.\n");
        free(node);
        depth--;
        return NULL;
    }
    int *right_indices = (int*)malloc(M * sizeof(int));
    if (!right_indices) {
        fprintf(stderr, "Failed to allocate space for image indices.\n");
        free(node);
        free(left_indices);
        depth--;
        return NULL;
    }
    int left_count = 0, right_count = 0;
    for (int i = 0; i < M; i++) {
        if (data->images[indices[i]].data[pixel] < 128) {
            left_indices[left_count] = indices[i];
            left_count++;
        } else {
            right_indices[right_count] = indices[i];
            right_count++;
        }
    }
    node->pixel = pixel;
    node->classification = -1;
    node->left = build_subtree(data, left_count, left_indices);
    if (!node->left) {
        free(left_indices);
        free(right_indices);
        free(node);
        depth--;
        return NULL;
    }
    node->right = build_subtree(data, right_count, right_indices);
    if (!node->right) {
        free(left_indices);
        free(right_indices);
        free(node);
        depth--;
        return NULL;
    }
    free(left_indices);
    free(right_indices);
    depth--;
    return node;
}

/**
 * This is the function exposed to the user. All you should do here is set
 * up the `indices` array correctly for the entire dataset and call 
 * `build_subtree()` with the correct parameters.
 */
DTNode *build_dec_tree(Dataset *data) {
    // TODO: Set up `indices` array, call `build_subtree` and return the tree.
    // HINT: Make sure you free any data that is not needed anymore
    int* indices = (int*)malloc(data->num_items * sizeof(int));
    if (!indices) {
        fprintf(stderr, "Failed to allocate space for image indices.\n");
        return NULL;
    }
    for (int i = 0; i < data->num_items; i++) {
        indices[i] = i;
    }
    DTNode* root = build_subtree(data, data->num_items, indices);
    free(indices);
    return root;
}

/**
 * Given a decision tree and an image to classify, return the predicted label.
 */
int dec_tree_classify(DTNode *root, Image *img) {
    // TODO: Return the correct label
    while (root->classification < 0) {
        if (img->data[root->pixel] < 128) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return root->classification;
}

/**
 * This function frees the Decision tree.
 */
void free_dec_tree(DTNode *node) {
    // TODO: Free the decision tree
    if (node) {
        free_dec_tree(node->left);
        free_dec_tree(node->right);
        free(node);
    }
    return;
}

/**
 * Free all the allocated memory for the dataset
 */
void free_dataset(Dataset *data) {
    // TODO: Free dataset (Same as A1)
    if (data) {
        if (data->images) {
            for (int i = 0; i < data->num_items; i++) {
                free(data->images[i].data);
            }
            free(data->images);
        }
        free(data->labels);
        free(data);
    }
    return;
}
