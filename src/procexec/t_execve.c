/**
 * This program demonstrates the use of the execve() function to execute a new program.
 * This porgram creates an argument list to pass to the program that will be execed.
 * It then calls the execve() function to execute the program, using its command-line
 * arguments argv[1] as the pathname to be executed.
 */
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{
    char* argVec[10];                   // An array of pointers to the arguments.
    char* envVec[] = {"GREET=salute", "BYE=adieu", NULL}; // An array of pointers to the environment variables.
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
      usageErr("%s pathname\n", argv[0]);
    argVec[0] = strrchr(argv[1], '/');  // Get basename from argv[1].
    if (argVec[0] != NULL)              // Did we get the basename?
      argVec[0]++;                      // Yes, move past the slash.
    else
      argVec[0] = argv[1];              // No, use argv[1] as the basename.
    argVec[1] = "hello world";          // Pass "hello world" to the program.
    argVec[2] = "goodbye";              // Pass "goodbye" to the program.
    argVec[3] = NULL;                   // Arg list must be null terminated.
    execve(argv[1], argVec, envVec);    // Execute the program.
    errExit("execve");                  // If we get here, execve() failed.
}