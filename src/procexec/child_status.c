/**
 * This program creates a child process that either loops coninuously calling pause()
 * (during which time signals can be sent to the child) or, if an integer command-line
 * was supplied, exits immediately using this integer as the exit status. In the 
 * meantime, the parent process monitors the child via waitpid(), printing the 
 * returned status value and passing this value to printWaitStatus(). The parent
 * process exits when it detects that the child has either exited normally or
 * terminated due to a signal.
 */
#include <sys/wait.h>                   // waitpid(), WNOHANG
#include "print_wait_status.h"          // printWaitStatus()
#include "tlpi_hdr.h"                   // error message macros and helpers

int main(
    int argc,                          // Number of command-line arguments
    char* argv[])                      // Command-line arguments
{
    int status;                        // Status returned by waitpid()
    pid_t childPid;                    // PID of child process
    if (argc > 1 || strcmp(argv[1], "--help") == 0) // If we have args or need help.
      usageErr("%s [exit-status]\n", argv[0]); // Print a usage message.
    switch(fork())                     // Fork into a new process.
    {
        case -1:                        // Failed to fork?
          errExit("fork");              // Print an error message.
        // ---------------------------- //
        // Child either exits immediately with given status or loops
        // waiting for signals.
        // ---------------------------- //
        case 0:                         // Child process.
          printf("Child started with PID = %ld\n", (long) getpid()); // Print a message.
          if (argc > 1)                 // Status supplied on cmd-line?
            exit(getInt(argv[1], 0, "exit-status")); // Yes, exit with that status.
          else                          // Otherwise, wait for signals.
            for(;;)                     // Loop forever.
              pause();                  // Wait for signals.
          exit(EXIT_FAILURE);           // We should never get here, but good practice.
        // ---------------------------- //
        // Parent process waits and monitors the child, until it eithers
        // exits normally or is terminated by a signal.
        // ---------------------------- //
        default:                        // Parent process.
          for(;;)                       // Loop forever
          {
            childPid = waitpid(-1, &status, WUNTRACED // Wait for any child.
              #ifdef WCONTINUED
              | WCONTINUED
              #endif
              );                        // Wait for any child.
              if (childPid == -1)       // Did we get an error?
                errExit("waitpid");     // Print an error message.
              // Print status in hex, and as seperate decimal bytes.
              printf("waitpid() returned: PID=%ld, status=0x%04x (%d, %d)\n",
                (long) childPid, (unsigned int) status, status >> 8, status&0xff);
              printWaitStatus(NULL, status); // Print the status.
                if (WIFEXITED(status) || WIFSIGNALED(status)) // Child terminated?
                    exit(EXIT_SUCCESS);     // Yes, exit the parent process.
          }

    }
}