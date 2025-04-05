/**
 * This program acheives the same result as the t_execve.c program, but uses the execlp() function
 * instead of the execve() function. The execlp() function is used to execute the program, and the
 * command-line arguments are passed as individual arguments to the function in the form of a 
 * list of strings.
 */
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{
    char* envVec[]={ "GREET=salute", "BYE=adieu", NULL }; // An array of pointers to the environment variables.
    char* filename;                      // The filename to execute
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
      usageErr("%s pathname\n", argv[0]);
    filename = strrchr(argv[1], '/');    // Get basename from argv[1].
    if (filename != NULL)                // Did we get the basename?
      filename++;                        // Yes, move past the slash.
    else
      filename = argv[1];                // No, use argv[1] as the basename.
    execle(argv[1], filename, "hello world", "goodbye", (char*) NULL, envVec); // Execute the program.
    errExit("execle");                   // If we get here, execle() failed.
}