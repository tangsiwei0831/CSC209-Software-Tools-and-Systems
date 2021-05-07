#include <stdio.h>

int main() {
    // gcc allows you to enter binary constants by prefacing the
    // the number with 0b:
    char a = 0b00010011;    // decimal value: 19

    // Similarly, preface with 0x for hexadecimal constants:
    unsigned char b = 0x14; // decimal value: 20

    // Negation:
    printf("result of negative %x is %x in hex\n", a, ~a);

    // Bitwise AND:
    printf("result of bitwise AND of %x and %x is %x in hex\n",
        a, b, a & b);

    // Bitwise OR:
    printf("result of bitwise OR of %x and %x is %x in hex\n",
        a, b, a | b);

    // Bitwise XOR:
    printf("result of bitwise XOR of %x and %x is %x in hex\n",
        a, b, a ^ b);

    return 0;
}
/*Since C is really a low-level programming language, we can read and modify each bit in a value directly. This allows us to work directly with raw data, ignoring the type of the variable. In another video you will see some examples where manipulating bits directly is useful. In this video we will focus on the tools -- the operators that C gives us to manipulate bits.

Let's first review the Boolean operations that are possible on a single bit. Recall that C represents false as the number 0 and true as 1.

From your work with conditional statements, you have already seen that C provides a logical and operator -- two ampersands. C also provides a bitwise and operator that is a single ampersand. For a single bit, the two operators look identical.

But if you look at values other than 0 and 1, you'll see that they operate differently.

Consider how numbers are represented in binary. The logical operator looks at the whole number and returns true if both arguments have a value other than 0. In contrast, he bitwise operator performs the AND operation on each pair of bits.

Similarly, although you're already familiar with logical or, which is two vertical bars, C also provides a bitwise or -- one bar. Like bitwise and, bitwise or works by performing the OR operation on each pair of bits.

The bitwise exclusive or operator is represented by a carat or hat symbol. It is 1 only if exactly one operand is 1.  The result is 0 if both operands are 0 or both operands are 1.

Exclusive or is sometimes called conditional negation: if the first operand is a 1 -- true -- then the result is the complement -- the negation -- of the second operand.

If the first operand is 0 -- false -- the result is the second operand WITHOUT being negated.

C also provides a bitwise negation -- or complement -- operator, the tilde. It takes a single value and flips every bit in the value. 1's become 0's, and 0's become 1's.

Be careful, though. Since it's a bitwise operator, while the bit 0 becomes the bit 1, the integer 0 will NOT become the integer 1.

The C representation of an integer 0 has many bits -- 32 of them, on my machine -- and negating it would result in the number with 32 1's!

Now that you know how each operator works, let's explore how to use them in a C program.

The first problem is how to store bits in a variable. While the C language doesn't define non-decimal constants, gcc allows you to enter binary constants by prefacing the number with 0b.

You can also use hexadecimal constants, prefaced with 0x. Hex is convenient because the binary numbers are usually large, and it isf pretty easy to translate from hexadecimal to binary, since one hexadecimal digit is 4 bits.

The binary number we wrote first is equal to hex 1-3. The first four bits are a 1. The last four bits are 2 + 1 = 3.

Let's look at a few of the operators using these values.  As we mentioned earlier, the bitwise operators apply to each bit of the variable.

So negating a will turn all 1's into 0's and all 0's into 1's.

So the result is hex E-C.

Each bit in hex 1-3 has been flipped.

Now let's look at the operators with two operands.

We can line up the two operands like we would multi-digit arithmetic, and apply the operator to each pair of bits.

Let's check that.

Can you predict the results of these statements?

Now that we know how to perform bitwise operations, we can start to use these operators in useful ways. In the next video, we'll look at facilities for shifting bits.*/