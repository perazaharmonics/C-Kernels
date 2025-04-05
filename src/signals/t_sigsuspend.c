/**
 * This program demonstrates the use of sigsuspend() function. It performs the following steps:
 * 1. Display the initial value of the process signal mask using printSigMask(). (1)
 * 2. Block SIGINT and SIGQUIT using sigprocmask(), and save original signal mask. (2)
 * 3. Establish the same handler for both SIGQUIT and SIGINT signals. (3) This handler
 *    displays a message, and if it was invoked via SIGQUIT, sets a global variable
 *    gotSigquit to 1.
 * 4. Loop until gotSigquit is true (4). Each loop iteration displays the current
 *    value of the signal mask using printSigMask(). Simulates a critical section by
 *    executing a CPU busy loop for a few seconds. Displays the mask of pending
 *    signals using printPendingSigs(). Finally, uses sigsuspend() to unblock SIGINT
 *    and SIGQUIT and wait for a signal to arrive (if one is not already pending).
 * 5. Use sigprocmask() to restore the original process signal mask to its 
 *    original state (5), and then display the signal mask using printSigMask(). (5)
 */

#define _GNU_SOURCE                 // Get strsignal() declaration from <string.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "signal_functions.h"       // Declaration of printSigMask() and printPendingSigs()
#include "tlpi_hdr.h"

static volatile sig_atomic_t gotSigquit =0; // Set to 1 if SIGQUIT is caught.
static void handler(int sig)            // Handler for signals.
{
    printf("Caught signal %d (%s).\n", sig, strsignal(sig));
    if (sig == SIGQUIT)                 // Caught SIGQUIT?
      gotSigquit = 1;                   // Yes. Set the flag to indicate that.
}

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    int nLoop;                          // Number of iterations until gotSigquit is true.
    time_t start;                       // Start time.
    sigset_t oMask;                     // The original process signal mask.
    sigset_t bMask;                     // The signal mask to block.
    struct sigaction sa;                // The signal action structure.

    /*(1)*/
    printSigMask(stdout, "Initial signal mask is:\n");
    sigemptyset(&bMask);                // Initialize and blank the block mask.
    sigaddset(&bMask, SIGINT);          // Add SIGINT to the block mask.
    sigaddset(&bMask, SIGQUIT);         // Add SIGQUIT to the block mask.
    /*(2)*/
    if (sigprocmask(SIG_BLOCK, &bMask, &oMask) == -1) // Block SIGINT and SIGQUIT.
      errExit("sigprocmask");
    sigemptyset(&sa.sa_mask);           // Clear the signal action struct.
    sa.sa_flags = 0;                    // No flags.
    sa.sa_handler = handler;            // Set the handler.
    /*(3)*/
    if (sigaction(SIGINT, &sa, NULL) == -1) // Establish the handler for SIGINT.
      errExit("sigaction");
    if (sigaction(SIGQUIT, &sa, NULL) == -1) // Establish the handler for SIGQUIT.
      errExit("sigaction");
    /*(4)*/
    for (nLoop =0; !gotSigquit; nLoop++) // Loop until SIGQUIT caught.
    {
        printf("+++ LOOP = %d +++.\n", nLoop);
        // ---------------------------- //
        // Simulate a critical section by delaying a few seconds.
        // ---------------------------- //
        printSigMask(stdout, "Starting critical section, signal mask is:\n");
        for (start = time(NULL); time(NULL) < start + 4;)
          continue;                     // Run the CPU busy loop.
        printPendingSigs(stdout,        // Print the pending signals.
          "Before sigsuspend() - pending signals:\n");
        if (sigsuspend(&oMask) == -1 && errno != EINTR) // Wait for a signal.
          errExit("sigsuspend");       // Handle errors.
    }
    /*(5)*/
    if (sigprocmask(SIG_SETMASK, &oMask, NULL) == -1) // Restore the original signal mask.
      errExit("sigprocmask");
    /*(6)*/
    printSigMask(stdout, "+++ Exited loop\nRestored signal mask to:\n");

    /*Do other processing...*/

    exit(EXIT_SUCCESS);                // Exit the program.

}
