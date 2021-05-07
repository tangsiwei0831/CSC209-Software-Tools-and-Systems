#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Write the copy function to perform exactly as strncpy does, with one
   exception: your copy function will guarantee that dest is always
   null-terminated. Capacity is expected to be the number of bytes of 
   memory allocated to dest.
   You shoud read the man page to learn how strncpy works.

  NOTE: You must write this function without using any string functions.
  The only function that should depend on string.h is memset.
 */

char *copy(char *dest, const char *src, int capacity) {
    int i = 0;
    for(i = 0; i < capacity; i++) {
        dest[i] = src[i];
    }
    while(i < capacity) {
        i += 1;
        dest[i] = '\0';
    }
    if(capacity > 0){
        dest[capacity - 1] = '\0';
    } 
    return dest;
}


int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: copy size src\n");
        exit(1);
    }
    int size = strtol(argv[1], NULL, 10);
    char *src = argv[2];

    char dummy1[size];
    char dest[size];
    char dummy2[size];
    
    memset(dest, 'x', size);

    // Set all the bytes of dest to a dummy value to facilitate testing
    // The two dummy arrays should be allocated before and after dest
    // We fill them with values to help with debugging and testing
    memset(dummy1, 'A', size);
    dummy1[size - 1] = '\0';
    memset(dummy2, 'Z', size);
    dummy2[size - 1] = '\0';

    copy(dest, src, size);

    printf("%s\n", dest);
    printf("%s %s\n", dummy1, dummy2);
    return 0;
}
