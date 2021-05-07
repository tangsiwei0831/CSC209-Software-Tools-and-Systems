#!/usr/bin/env bash
# Run echo_arg with the command-line argument csc209 and redirect the output to the file echo_out.txt.
./echo_arg csc209 > echo_out.txt
# Run echo_stdin with its standard input redirected from the file echo_stdin.c.
./echo_stdin < echo_stdin.c
# Use a combination of count and the Unix utility program wc (Links to an external site.) to determine the total number of digits in the decimal representations of the numbers from 0 to 209, inclusive.
./count 210 | wc -m
#Use a combination of echo_stdin and ls (Links to an external site.) to print out the name of the largest file in the current directory. You can assume the largest file has a name with fewer than 30 characters.
ls -S | ./echo_stdin