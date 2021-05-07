#include <stdio.h>
#include <stdlib.h>
#include "knn.h"

unsigned char dataset[MAX_SIZE][NUM_PIXELS];
unsigned char labels[MAX_SIZE];
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }
    load_dataset(argv[1], dataset, labels);
    return 0;
}
