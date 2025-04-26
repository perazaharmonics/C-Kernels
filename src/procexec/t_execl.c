/**
 * This program demonstrates that the new program inherits its environment from the caller
 * during an execl() call. This program first uses putenv() to make a change in the environment
 * it inhertits from the shell as a result of fork(). Then the printenv program is executed
 * to display the values of the USER and SHELL environment variables.
 */
#include <stdlib.h>
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{
    printf("Initial value of USER: %s\n", getenv("USER")); // Print the initial value of USER.
    printf("Initial value of SHELL: %s\n", getenv("SHELL")); // Print the initial value of SHELL.
    if (putenv("USER=britta") != 0)    // Change the value of USER.
      errExit("putenv");
    execl("/usr/bin/printenv", "printenv", "USER", "SHELL", (char*) NULL); // Execute the printenv program.
    errExit("execl");                   // If we get here, execl() failed.
}