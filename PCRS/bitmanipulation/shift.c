#include <stdio.h>

// Return var with the kth bit set to 1
unsigned char setbit(unsigned char var, int k) {
	return var | (1 << k);
}

// Return 0 if the kth bit in var is 0,
// and a non-zero value if the kth bit is 1
int checkbit(unsigned char var, int k) {
	return var & (1 << k);
}

int main() {
    unsigned char b = 0xC1; //1100 0001 in binary

	printf("Original value %x\n", b);

	// set the third bit of b to one
	b = b | 0x8; // 0x8 == 0000 1000
	printf("With the third bit set %x\n", b);

	// check if the second bit of b is set
	// 1100 1001
	if(b & 0x4) { // if the result of bitwise AND-ing b with 0x8 is not zero
		printf("The second bit of %x is 1\n", b);
	} else {
		printf("The second bit of %x is 0\n", b);
	}
	
    return 0;
/*In the first video on bit manipulation, you learned about the bitwise operators in C. To make use of these operators, let's explore a couple of problems.

Given a variable b, set the third bit to 1 and leave the other bits unchanged.

First, given a variable b, set the third bit to 1. No other bits should be changed.

We count bits from right to left starting at position 0.  So the bit at position 3 in our variable b has the value 0.

We can use the bitwise or operator here. 1 or 0 is 1. 1 or 1 is also 1.

Let's use the value 8. In binary representation, the value 8 has a 1 at the third bit. Using bitwise or, any value we combine with 8 will end up with a 1 at the 3rd bit.

Now let's tackle another problem. Given a variable b, check if the second bit is has the value 1.

We will use the & operator, because the only way that bitwise and results in 1 is if both bits are 1.

We use the value 4, since it has a 1 at the position we want to check. If the second bit of b were 1, then 1 & 1 would be 1, but in our example, the second bit is 0, so 1 and 0 is 0.

Suppose we want to make these problems more general. Our solutions so far rely on us knowing exactly which position we want to check, so now we need a way to create a value with an arbitrary bit set to one.

For that will use C's shift operator.  There are actually two shift operators -- shift left and shift right. Think of them as arrows pointing in the direction of the shift.

The shift operators take two operands, the first operand -- to the left of the operator -- is the value to shift, and the second -- to the right -- tells us how many places to shift.

If we shift the value 1 to the right 3 positions, we end up with ... 2 ... 4 ... 8. So, by replacing the second operand with a variable, we can generate a value with any singel bit set. This allows us to solve our general problems.

To set the kth bit of var, we will bitwise-or var with 1 shifted k times.

And similarly we can check if the kth bit is set by bitwise anding var with 1 shifted k times.

These two operations -- setting and checking a single bit -- are very common. It's worth learning these useful programming idioms.

However, we can use the shift operators on values other than 1. In this example, we shift a 1 byte value -- that is, an 8 bit value -- to the left by one position. The leftmost bit is discarded and the rightmost bit is set to 0. The other bits are shifted 1 bit to the left.

If we shift by 2, then the leftmost two bits are discarded, the rightmost two bits are set to 0, and the other bits are shifted 2 to the right.

Here's another way to think about the left shift operator. Look at what happens whenever we shift a number one bit to the left. I'm sure you can see the pattern.  It's the same as multiplying the number by two.

We have focused on the left shift operator in this video because it was useful for our problem, but the right shift operator is also useful -- and works in the same way. In this example, you can see that right shift works like integer division.



In this video, we will explore the use of flag bits. Flag bits are commonly used by system calls, when a single argument is used to transmit data about multiple options. That variable is treated as an array of bits, where each bit represents an option -- or flag -- that can be turned on and off. We will explore how this concept is used to implement file permissions.

Let's look at how file permissions work in Linux. Here is the contents of a directory.

The first column in the output is the permission string. This represents who can read, write, or execute the file.

The third column is the owner of the file, and the fourth column is the group.

The owner and group fields are relevant to permissions because Linux allows us to set separate permission for the owner of the file, the group, and every other user in the system.

Let's look more closely at the permissions for syscall_cost.

The leftmost dash in the permission string identifies the file type. The dash means that it is a regular file. "results" which is a directory, has a d in this position, and a link would have an l.

The owner of the file  has read, write, and execute permissions on the file.

The members of the group instrs have read and execute permissions on the file but cannot write the file.

Everyone else also has read and execute permissions.

How does this relate to bit manipulation operators and flag bits? If you ignore the first dash that represents the file type, each file permission is essentially an on/off switch, so we can represent each one with a flag bit.

We need nine bits to represent each permissions setting, so we need a variable with enough space for 9 bits. This rules out storing it in a byte.  The system calls that operate on file permissions use a mode_t type for the permissions string.

On my system, this is defined as an unsigned int -- a 32-bit value.

The permissions will be stored in the 9 low-order bits -- or rightmost -- bits.

The first step is to see how to turn the permissions string into a sequence of bits.  Each character in the permissions string becomes a bit in our variable representing the permissions.

Bits 8, 7, 6 represent the read, write, and execute permissions for the user.

Bits 5, 4, and 3 represent the permissions for the group, and bits 2, 1, and 0 represent the permissions for everyone else.

What if we wanted to represent a file with read-only permissions?

The permissions string for a read-only file would be r--r--r--

So we'd represent it with the binary value 100100100. As we know from the past few videos, we could construct that value with bitwise operators. Fortunately, the system calls that take a permissions varaible as an argument use several constants to make it easier for us to construct these values.

One of the system calls that operates on file permissions is the chmod system call. Looking at the man page, we see that the arguments to chmod are the path to file we want to set permissions for, and the mode or permissions to set.

Looking farther down the man page, we see a set of defined constants which will make it easier to set a value for the mode.

Note that these constants are defined in octal or base 8.  Base 8 is convenient for representing permissions because one digit in base 8 is three binary digits.

An octal or base 8 constant in C is written with a preceding 0.

The library defines constants for each possible permission. Here, for example, are the three constants for setting the read permission.

Now, we can go back to our problem.

We can create the permission value using the bitwise operators we learned about in previous videos. In this case, to set bits, we use bitwise OR with the appropriate constants.

Similarly, we can check if bits are set using bitwise AND.

In this case, we combine the two flags for the group and other users using bitwise OR. Then, we use bitwise AND to check if either bit is set.

Flag bits are a compact, efficient method for representing data with multiple boolean components. The use of individual bits, rather than whole characters or integers, saves space, while the use of bitwise operators is clean and fast.*/