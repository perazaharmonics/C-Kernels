/**
 * This program demonstrates the manipulation of the close-on-exec flag. Depending on the presence
 * of a cmd-line argument (any string), this program first sets the close-on-exec flag
 * for standard output and then execs the ls program.
 */
#include <fcntl.h>                      // For fcntl()
#include "tlpi_hdr.h"                   // For error messages

int main(                               // ----------- main ----------------- //
    int argc,                           // Number of cmd-line arguments.
    char* argv[])                       // Array of cmd-line arguments.
{
    int flags;                          // File status flags.
    if (argc > 1)                       // Was a cmd-line arg specified?
    {                                   // Yes.
        flags = fcntl(STDOUT_FILENO, F_GETFD);// Get the file status flags.
        if (flags == -1)                // Did fcntl() fail?
            errExit("fcntl - F_GETFD"); // Yes, print an error msg.
        flags |= FD_CLOEXEC;            // Then, turn on the close-on-exec flag.
        if (fcntl(STDOUT_FILENO, F_SETFD, flags) == -1)// Could we set the flags?
            errExit("fcntl - F_SETFD"); // No, print an error msg.
    }                                   // Done getting user input.
    execlp("ls", "ls", "-l", (char*) NULL);// Execute the ls program.
    errExit("execlp");                  // If we get here, execlp() failed.
}                                       // ----------- main ----------------- //