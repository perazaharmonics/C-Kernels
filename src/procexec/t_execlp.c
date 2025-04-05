/**
 * This program demonstrates the use of execlp() to execute a new program.
 * The program creates an argument list to pass to the program that will be execed.
 * It then calls the execlp() function to execute the program, using its command-line.
 */
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{
    if (argc !=  strcmp(argv[1], "--help") == 0)
      usageErr("%s pathname\n", argv[0]);
    execlp(argv[1], "hello world", "goodbye", (char*) NULL); // Execute the program.
    errExit("execlp");                  // If we get here, execlp() failed.
}