/**
 * This is the program to be executed by the execve() function in the t_execve.c program.
 * This program simply prints its command-line arguments and environment variables.
 * The latter which is accessesed via the global variable environ.
 */
#include "tlpi_hdr.h"                   // TLPI header file.

extern char** environ;                  // The global environment.

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{
    int j;                              // Loop counter.
    char** ep;                          // Pointer to env var list.
    for (j = 0; j < argc; j++)          // Loop through the arguments.
      printf("argv[%d] = %s.\n", j, argv[j]); // Print the argument.
    for (ep = environ; *ep != NULL; ep++) // Loop through the environment variables.
      printf("environ: %s.\n", *ep);    // Print the environment variable.
    exit(EXIT_SUCCESS);                 // Exit the program.
}