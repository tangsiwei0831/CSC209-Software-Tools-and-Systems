#include <stdio.h>
#include <stdlib.h>
#include "knn.h"

unsigned char image1[NUM_PIXELS];
unsigned char image2[NUM_PIXELS];

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s imagefile\n", argv[0]);
        exit(1);
    }
    load_image(argv[1], image1);
    load_image(argv[2], image2);
    printf("%f \n",distance(image1, image2));
    return 0;
}