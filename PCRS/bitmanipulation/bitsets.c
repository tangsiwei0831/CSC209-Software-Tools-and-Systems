#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define INTSIZE 32
#define N 4

struct bits {
    unsigned int field[N];
};

typedef struct bits Bitarray;

/* Initialize the set b to empty or all zeros
 * Return 0 if memset fails.
 */
int setzero(Bitarray *b){
  return (memset(b, 0, sizeof(Bitarray)) == NULL);
}

/* Add value to the set b
 *   (set the bit at index value in b to one)
 */
void set(unsigned int value, Bitarray *b) {
    int index = value / INTSIZE;
    b->field[index] |= 1 << (value % INTSIZE);
}

/* Remove value from the set b
 *   (set the bit at index value in b to zero)
 */
void unset(unsigned int value, Bitarray *b) {
    int index = value / INTSIZE;
    b->field[index] &= ~(1 << (value % INTSIZE));
}

/* Return true if value is in the set b, and false otherwise.
 *    Return a non-zero value if the bit at index 'value' is one in b
 *    Return zero if the bit at index 'value' is zero in b
 */
int ifset(unsigned int value, Bitarray *b) {
    int index = value / INTSIZE;
    return ( (1 << (value % INTSIZE)) & b->field[index]);
}

/* Run some simple tests on the above functions*/
int main() {

    Bitarray a1;
    setzero(&a1);

    // Add 1, 16, 32, 65 to the set
    set(1, &a1);
    set(16, &a1);
    set(32, &a1);
    set(68, &a1);

    // Expecting: [ 0x00010002, 0x00000001, 0x000000010, 0 ]
    // Print using hexadecimal
    printf("%x %x %x %x\n",
            a1.field[0], a1.field[1], a1.field[2], a1.field[3]);

    unset(68, &a1);

    // Expecting: [ 0x00010002, 0x00000001, 0, 0 ]
    // Print using hexadecimal 
    printf("%x %x %x %x\n",
            a1.field[0], a1.field[1], a1.field[2], a1.field[3]);

    return 0;
}
/*
Previously, we looked at the implementation of flags using arrays of bits. We can use the same concept for a different purpose. In this video, we'll explore the use of bit flags to implement a *set*. If we use each bit in a variable to denote the presence -- or absence -- of a particular element in the set, then we have a very compact implementation that can use fast boolean instructions as set operations.

We are going to look at the particular case where the possible set elements are represented by small positive integers.

For example, you might want to store colours in a set.  By giving each colour a numeric code, so that red is 2, and purple is 7 for example, we can then store the codes in the set.

When our set contains small positive integers, we can represent the set as an array of bits where the elements of the set are the indexes into the array and the value at a given index location tells us whether the element is in the set. In our example, we can see that 7, 5, 3 and 2 are in the set because the bit array contains ones at these indexes.

To add another element, say 10, to bit_array we first use the shift operator to create a value where the 10th bit is 1 and all the other bits are 0.  Bitwise ORing it with bit_array adds 10 to the set.

What you've just seen is a common programming technique called *bit masking*. We are creating a *bit mask* -- a carefully constructed value where specific elements are set or not set -- and then applying the mask to set or unset those values.

Here's another example.

To remove 10 from the set, we want to bitwise AND a zero in the 10th bit with bit_array, but if the other bits are zero, that would have the side effect of removing all items from the set. So, we need to create a value where all of the bits are one except the 10th bit, and bitwise AND that value with bit_array.

In this example, we used an unsigned short to hold the bit array. This means that the range of values the set can contain is 0 to 15. It isn't hard to imagine that we want sets that hold values larger than 15.

We can double the range of the set by changing the type of our variable to an unsigned int, but even 31 is pretty small to be the maximum value.

We can generalize the data structure that we use to store the bits by making an array of unsigned ints.

Of course, the operations to add a value to a set and remove a value from the set are a little more complicated now, so let's look at an example of how to identify a particular bit in the bit array.  Suppose we want to set the bit at index 34 to 1.  The first step is to figure out which of the unsigned ints in the array contains bit 35.

Since each unsigned int contains 32 bits, we use integer division to divide 34 by 32 to get the index into the array of unsigned ints -- 1 in this case.

The next step is to find the index of the bit in the unsigned int.  We use the mod operator to find the remainder of the integer division from the previous step.  34 % 32 is 2.

To set the bit at index 34 to 1, we combine the operations from the simple example earlier in the video. Now let's put it all together and fill in the code to implement the set operations.

We will make one more alteration to the data structure, and wrap the array in a struct.

There are two benefits to wrapping the array in a struct. It hides the implementation of the set, and it allows us to use a simple assignment statement to make a copy of the set. Now that we are using an array of unsigned ints, we can choose an arbitrary value for  N, and create sets where the range of possible values in the set is quite large.

setzero is the intialization function. It uses the memset library function to fill the array with zeros.

set is the first function to implement.

Here's the code we developed earlier for a specific example.

Let's reduce it a bit with the or-equals operator.

We need to update it since we're now using a pointer to a struct to represent the set.

The index into the BitArray is the value we want to add to the set.

And we need to divide by the number of bits in each element of the Bitarray.

We will take a similar approach to unset.

The key difference between set and unset is that for unset, we need to make sure that every bit is a 1 except for the value to be unset.

So, we'll set the bit we want to a 1 and then negate the whole value. And then we'll use the AND operator rather than OR.

Now let's implement the function that checks if a bit is set. This requires a combination of the operations from set and unset.

Getting the bit to check is the same as before.

Then, we use AND to check whether it is set in the original value. If the bit was in the set, then the resulting value will not be 0 because one bit will be a one.  But if the bit was not in the set the resulting value will be 0 because 1 & 0 is 0.

Finally, we add a main function to make some quick tests of our functions.  This is not an exhaustive set of tests, but at least show whether the basic idea is correct. Note that there isn't a straight forward way to print a number in binary, so the tests print the values in the field array in hexadecimal format.*/