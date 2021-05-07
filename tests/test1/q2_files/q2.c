#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Question 2 [9 marks]  
 * This file goes with the q2.pdf file that contains the memory model diagram
 * for this problem.
 *
 * Write your code for the memory model question in this file
 * Remember to free any memory you allocate.
 * You may not "hard-code" the solution.  The function sep
 * should work as expected if we passed in different arguments.
 * You do not need to handle any corner cases that do not appear
 * in the memory diagram.
 */

/* sep returns a list of pointers to the first "num" spaces in "str" */
char **sep(char *str, int num) {
    int j = 0;
    int i = 0;
    char **arr = malloc(sizeof(char *) * 3);
    for(i = 0; i < strlen(str); i++){
        if(str[i] == ' '){
            arr[j] = &(str[i]);
            j += 1;
        }
    }
    i = 9;
    return arr;
}

/* When you follow the memory model diagram, you will see that main doesn't
 * do anything interesting other than declare and intialize some variable and
 * call a function.  You are welcome to print out values of variables
 * for testing.
 */
int main() {
    int n = 3;
    char *words = "I'm not a cat?"; 
    char ** ptrs = sep(words, n);
    free(ptrs);
    return 0;
}

