/**
 * This program sets up a signle handler to catch all signals (2). Excepts SIGKILL
 * and SIGSTOP which are impossible to catch. For most types of signal the handler (1)
 * simply counts the signal using an array. If SIGINT is received, the handler sets 
 * a flag (gotSigInt) that causes the program to exit its main loop. If a cmd-line
 * was supplied to the program, then the program blocks all signals, for the # of
 * seconds specified by that argument, and then, prior to unblocking the signals,
 * displays the set of pending signals (3). This allows us to send signals to the
 * process before it commences the following step. In the following step the program
 * executes a while loop that consumes CPU time until gotSigint is set (4).
 * after executing the while loop, the program displays counts of all signals
 * received (5).
*/

#define _GNU_SOURCE
#include <signal.h>
#include "signal_functions.h"
#include "tlpi_hdr.h"

static int sigCount[NSIG];              // The # of delivery of each signal.
static volatile sig_atomic_t gotSigint=0;// Non-zero if SIGINT delivered.

// (1) Signal Hanlder
static void handler(                    // Simple signal handler.
  int sig)                              // The signal received.
{
  if (sig == SIGINT)                    // Did we receive a interrupt signal?
    gotSigint=1;                        // Record that.
  else                                  // Else, must be a different signal.
    sigCount[sig]++;                    // Count it.
}

// Our main program displaying how the program reacts and how to handle
// multiple signals.
int main(
  int argc,                             // The number of signals.
  char* argv[])                         // The variable argument list.
{
  int n;                                // The signal counter.
  int nSecs;                            // The number of seconds to sleep.
  sigset_t pendingMask;                 // The bit mask for pending signals.
  sigset_t blockingMask;                // The bit mask for blocked signals.
  sigset_t emptyMask;                   // An empty signal bit mask.
    // Show the process ID to the user of this program.
  printf("%s: PID is %ld.\n", argv[0], (long) getpid());
  
    /* (2) Same handler to catch all signals. */
  for (n = 1; n < NSIG; n++)           // For every possible signal
    (void) signal(n, handler);          // Ignore errors.
    // -------------------------------- //
    // If a sleep time was specified, temporarily block all signals.
    // Sleep (while another person sends us signals), then
    // display the mask of pending signals, and unblock all signals.
    // -------------------------------- //
    /* (3)                              */
  if (argc > 1)                         // More than one argument?
  {                                     // Yes.
    nSecs=getInt(argv[1], GN_GT_0, NULL);// Get the # of secs to sleep for.
    sigfillset(&blockingMask);          // Initialize the blockingMask set.
    // -------------------------------- //
    // Can we change the process signal to blocking while it sleeps?
    // -------------------------------- //
    if (sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
      errExit("sigprocmask");           // No, that's an error.
    // -------------------------------- //
    // Else, notify we are sleeping.
    // -------------------------------- //
    printf("%s: sleeping for %d second"
      "s.\n",argv[0],nSecs);
    sleep(nSecs);                       // Sleep for nSecs.
    if (sigpending(&pendingMask) == -1) // Could we catch the pending signals?
      errExit("sigpending");            // No, that's an error.
    printf("%s: pending signals are: \n",
      argv[0]);
    // -------------------------------- //
    // Print the signal set.
    // -------------------------------- //
    printSigset(stdout, "\t\t", &pendingMask);  
    // -------------------------------- //
    // Unblock the pending signals.
    // -------------------------------- //
    sigemptyset(&emptyMask);             // Blank out the signal set struct.
    // -------------------------------- //
    // Can we unblock the process pending signals ?
    // -------------------------------- //
    if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
       errExit("sigprocmask");          // No, that's an error.          
  }                                     // Done handling the sleep argument.
  /* (4)                                */
  while (!gotSigint)                    // Loop until SIGINT is caught.
    continue;                           // 
  /* (5)                                */
  for (n = 1; n < NSIG; n++)           // For every possible signal
    if (sigCount[n] != 0)               // Did we send this signal?
      printf("%s: signal %d caught %d " // Yes, print that.
        "time%s.\n", argv[0], n, sigCount[n],
         (sigCount[n] == 1) ? "" : "s");
  exit(EXIT_SUCCESS);                   // If we got here, then we did it.
}

