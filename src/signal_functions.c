#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "signal_functions.h"
#include "tlpi_hdr.h"

/**
A signal handling helper to print signal set information and pending signals.
* NOTE: These functions use fprintf(), which is not async-sinal safe. As such,
* these functions are also not async-signal-safe (i.e. beware of using them
* indiscriminately with signal handlers.)
*/

// Print list of signal within a signal set.
void printSigset(
  FILE* of,                             // Output file
  const char* prefix,                   // Message prefix
  const sigset_t* sigset)               // The set of signals
{
  int sig;                              // The signal in question.
  int count;                            // The signal count.
  count=0;                              // Initialize counter
  for (sig = 1; sig < NSIG; sig++)      // For every possible signal
  {
    if (sigismember(sigset, sig))       // Does the signal belong to this set?
    {                                   // Yes, count it.
      count++;                          // Yes, count it.
      fprintf(of, "%s%d (%s)\n", prefix,// and print it.
         sig, strsignal(sig));
    }                                   // Done printing signal from set.
  }                                     // Done with every possible signal.
  if (count == 0)                       // Did we count any signals?
    fprintf(of, "%s<empysignal set>.\n",prefix);
}

// Print mask of blocked signals for this process.
int printSigMask(
  FILE* of,                             // The output file.
  const char* msg)                      // The message to print.
{
  sigset_t currMask;                    // The current signal mask.
  if (msg != NULL)                      // Anything to print?
    fprintf(of, "%s", msg);             // Print it.
  // Mask not part of of signals block for this process?
  if (sigprocmask(SIG_BLOCK, NULL, &currMask) == -1)
    return -1;                          // No, so exit.
  
  printSigset(of, "\t\t", &currMask);   // Else, print it.
  return 0;
} 

// Print signals currently pending for the process.
int printPendingSigs(
  FILE* of,                             // The output file.
  const char* msg)                      // The message to print.
{
  sigset_t pendingSigs;                 // The signals pending.
  
  if (msg != NULL)                      // Anything to print?
    fprintf(of, "%s", msg);             // Yes, print it.
  if (sigpending(&pendingSigs) == -1)   // Signals pedning could be obtained?
    return -1;                          // No, return -1.
  // Else, some signals are pending...
  printSigset(of, "\t\t", &pendingSigs);// Print them.
  return 0;                             // Success.
}
