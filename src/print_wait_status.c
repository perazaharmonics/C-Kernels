/**
 * This function uses the macros in the sys/wait.h header file to monitor the termination of child processes,
 * and prints a message after decoding the status returned by waitpid().
 * 
 */
#define _GNU_SOURCE                     // Get strsignal() declaration from <string.h>
#include <string.h>                     // strsignal()
#include <sys/wait.h>                   // waitpid(), WNOHANG
#include "print_wait_status.h"           // Declaration of printWaitStatus()

/**
 * NOTE: The following function employs printf() which is not
 * async-signal-safe. As such, this function is not async-signal-safe.
 * (i.e, beware calling it from a signal handler.)
 */
void printWaitStatus(
    const char* msg,                    // Message to print
    int status)                         // Status returned by waitpid()
{
    if (msg != NULL)                    // Did we provide a msg?
      printf("%s", msg);                // Yes, print it.
    if (WIFEXITED(status))              // Child exited normally?
      printf("child exited with status %d.\n", WEXITSTATUS(status)); // Yes, print the status.
    else if (WIFSIGNALED(status))        // Child was terminated by a signal?
    {                                    // Yes, print the signal.
      printf("child killed by a signal %d (%s).\n",
        WTERMSIG(status), strsignal(WTERMSIG(status))); // Yes, print the signal.
#ifdef WCOREDUMP                        // WCOREDUMP is defined?
        if (WCOREDUMP(status))          // Child produced a core dump?
          printf(" (core dumped)");     
#endif                                  // End of WCOREDUMP block.
        printf("\n");                   // Print a newline character.
    }                                   // End signal kill block.
    else if (WIFSTOPPED(status))        // Child was stopped by as signal?
    {
        printf("child stopped by signal %d (%s).\n",
          WSTOPSIG(status), strsignal(WSTOPSIG(status))); // Yes, print the signal.
#ifdef WIFCONTINUED                     // WIFCONTINUED is defined?
    }
    else if (WIFCONTINUED(status))      // Child was resumed by delivery of SIGCONT?
    {
        printf("child continued.\n");   // Yes, print a message.
#endif                                  // End of WIFCONTINUED block.
    }
    else                                // We should never get here.
    {
        printf("what happened to this child? (status=%x)\n",
          (unsigned int) status);        // Print an error message.
    }

}