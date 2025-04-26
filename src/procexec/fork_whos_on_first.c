/**
 * This program demonstrate the indeterminate order in which the parent and child
 * processes are scheduled to run after using fork(). It loops using fork() to create
 * multiple children. After each fork(), both parent and child print the message
 * containing the loop counter and a string identifying if its a parent or child.
 *  
 */
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    int nChildren;                      // The number of children to create.
    int j;                              // The loop counter.
    pid_t childPid;                     // The child process ID.
    if (argc > 1 && strcmp(argv[1], "--help") == 0) // User wants help?
      usageErr("%s [num-children.].\n", argv[0]); // Yes, display help.
    nChildren = (argc > 1) ? getInt(argv[1], GN_GT_0, "num-children") : 1; // Get the number of children.
    setbuf(stdout, NULL);               // Disable buffering of stdout.
    for (j = 0; j < nChildren; j++)     // Loop to create children.
    {
        switch(childPid = fork())       // Create a child process.
        {                               // Begin processing child and parent.
          case -1:                      // Error forking?
            errExit("fork");            // Yes, handle the error.
          case 0:                       // Our child.
            printf("%d child.\n", j);   // Print the child message.
            _exit(EXIT_SUCCESS);        // Exit the child.
          default:                      // The parent.
            printf("%d parent.\n", j);  // Print the parent message.
            wait(NULL);                 // Wait for the child to exit.
            break;                      // Break out of the switch.
        }
    }
    exit(EXIT_SUCCESS);                 // Exit the program.
}