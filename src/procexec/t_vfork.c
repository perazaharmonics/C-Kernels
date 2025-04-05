/**
 * This program demonstrates the use of vfork(), both its semantic features that
 * differ from fork(): the child shares the parent's address space or memory, and   
 * the parent is suspended until the child calls exec() or terminates with _exit().
 */
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{                                       // -------------------------------- //
    int istack = 222;                   // The integer stack variable.
    switch (vfork())                    // Create a child process.
    {                                   // Begin processing the child and parent.
        case -1:                        // Error forking?
          errExit("vfork");             // Yes, handle the error.
        case 0:                         // Our child.
    // -------------------------------- //
    // Child process executes first, in parent's address space.
    // Even if we sleep for a while, parent is still not scheduled.
    // -------------------------------- //
          sleep(3);                     // Sleep for 3 seconds.
          write(STDOUT_FILENO, "Child executing.\n", 16); // Write the message.
          istack *=3;                   // This change is seen by parent.
          _exit(EXIT_SUCCESS);          // Exit the child.
        default:                        // The parent, who's blocked until child exits
          write(STDOUT_FILENO, "Parent executing.\n", 17); // Write the message.
          printf("istack=%d\n", istack); // Print the integer stack variable.
          exit(EXIT_SUCCESS);           // Exit the program.                      
    }
}