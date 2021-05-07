#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Complete this program by writing the function strip_q_marks that takes
    a single string and returns an integer.

    The function should modify the string to remove any trailing question marks
    and return the number of question marks that were removed.

	Note that you should put the commandline argument in double quotes when you
	type it in.  This prevents the shell from interpreting characters such as "?"
	or a space as special characters, and passes them to the program as is.

	For example, you would type:
	./strip "Hello? World???" 

    Examples
    original sentence       modified sentence       return value
    =============================================================
    "Hello? World???"       "Hello? World"          3
    "What..?"               "What.."                1
    "Apples?..?"            "Apples?.."             1
    "Coffee"                "Coffee"                0
    "Who?What?Where?"       "Who?What?Where"        1
*/

// Write the function strip_q_marks here
int strip_q_marks(char* s){
    int res = 0;
    int og = strlen(s) - 1;
    while (s[og] == '?'){
        s[og] = '\0';
        res += 1;
    }
    return res;
}

int main(int argc, char **argv) {
    // Do not change this main function.
    if(argc != 2) {
        fprintf(stderr, "Usage: strip message\n");
        exit(1);
    }
    int result = strip_q_marks(argv[1]);
    printf("%s %d", argv[1], result);
    return 0;
}
