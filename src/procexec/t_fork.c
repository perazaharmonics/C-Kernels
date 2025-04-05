/**
 * This program demonstrates the use of fork(). It creates a child that modifies the copies of global
 * and automatic variables that it inherits during the fork(). The use of sleep() in the code executed
 * by the parent permits the child to be scheduled for the CPU before the parent, so that the child can
 * complete its work and terminate before the parent continues execution. Using sleep in this manner
 * is not foolproof method of guaranteeing this result.
 */
#include "tlpi_hdr.h"

static int idata = 111;     // Allocated in data segment.

int main (
    int argc,               // The argument count.
    char* argv[])           // The variable argument list.
{
    int istack = 222;       // Allocated in stack frame segment.
    pid_t childPid;         // The child process ID.
    switch (childPid = fork()) // Create a child process.
    {
        case -1:            // Error forking?
          errExit("fork");  // Yes, handle the error.
        case 0:             // Child process.
          idata *=3;        // Modify the global variable.
          istack *=3;       // Modify the stack variable.
          break;            // Exit the switch.
        default:            // Parent process.
          sleep(3);         // Give the child a chance to execute.
          break;            // Exit the switch.
    }
    // Both parent and child come here.
    printf("PID=%ld %s idata=%d istack=%d.\n", (long) getpid(),
      (childPid == 0)? "(child)" : "parent", idata, istack);
    exit(EXIT_SUCCESS);   // Exit the program.
}