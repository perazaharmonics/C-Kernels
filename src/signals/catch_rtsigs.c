/**
 * This program catches signals and displays various fields from the siginfo_t structure passed
 * to the signal handler. The program takes two optional integer command-line arguments. If
 * the first argument is supplied, the main program blocks all signals and then sleeps
 * for a number of seconds specified by this argument. During this time, we can queue
 * multiple realtime signals to the process and observe what when the signals are
 * unblocked. The second argument specifies the number of seconds that the signal
 * handler should sleep before returning. Specifying a nonzero value (default 1 sec)
 * is useful for slowing down the program so that we can see more easily when multiple
 * signals are handled.
 * Use in combination with the t_sigqueue program to send signals to this program.
 * See page 461 of the Linux Programming Interface book for more information.
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static volatile int sleepFor;            // Number of seconds to sleep in handler.
static volatile int sigCount=0;          // Number of signals received.
static volatile sig_atomic_t allDone = 0;// Set to 1 once SIGINT is caught.

// Hanlder for signals established using SA_SIGINFO flag.
static void handler(
    int sig,                              // The signal number.
    siginfo_t* si,                        // Signal information.
    void* ucontext)                       // The user context.
{
    // UNSAFE: This handler uses non-async-signal-safe functions (printf())
    // SIGINT or SIGTERM can be used to terminate the program.
    if (sig == SIGINT || sig == SIGTERM) // Received SIGINT or SIGTERM?
    {                                    // Yes
      allDone=1;                         // Set the allDone flag.
      return;                            // We're done.
    }
    sigCount++;                           // Increment the signal count.
    printf("Caught signal %d.\n", sig);   // Print the signal caught.
    printf("      si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
      (si->si_code == SI_USER) ? "SI_USER" :
      (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    printf("si_value=%d.\n", si->si_value.sival_int);
    printf("    si_pid=%ld, si_uid=%ld.\n", (long) si->si_pid, (long) si->si_uid);
    if (sleepFor != 0)                   // Should we sleep?
      sleep(sleepFor);                   // Yes, sleep.
}

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    struct sigaction sa;                // The signal action structure.
    int sig;                            // The signal number.
    sigset_t prevMask;                  // The previous signal mask.
    sigset_t blockMask;                 // The signal mask to block. 
    if (argc > 1 && strcmp(argv[1], "--help") == 0) // Did we ask for help?
      usageErr("%s [block-time [handler-sleep-time]]\n", argv[0]); // Print help.
    printf("%s: PID is %ld.\n", argv[0], (long) getpid()); // Print the PID.
    sleepFor = (argc > 2) ? getInt(argv[2], GN_NONNEG, "handler-sleep-time") : 1;
      // -------------------------------- //
      // Establish handler for most signals. During execution of the handler,
      // mask all other signal to prevent handlers recursively interrupting
      // each other (which would make the output hard to read).
      // -------------------------------- //
    sigemptyset(&sa.sa_mask);           // Clear the signal mask.
    sa.sa_sigaction = handler;          // The signal handler.
    sa.sa_flags = SA_SIGINFO;           // Use the sa_sigaction field.
    sigfillset(&sa.sa_mask);            // Fill the signal mask.
    for (sig = 1; sig < NSIG; sig++)    // For all signal.
      if (sig!= SIGTSTP && sig != SIGQUIT)// Not SIGTSTP or SIGQUIT?
        sigaction(sig, &sa, NULL);
      // ------------------------------ //
      // Optionally block signals and sleep, allowing signals to be sent to us
      // before they are unblocked and handled.
      // ------------------------------ //
    if (argc > 1)                       // Did we specify a block time?
    {                                   // Yes.
        sigfillset(&blockMask);         // Fill the signal mask.
        sigdelset(&blockMask, SIGINT);  // Remove SIGINT from the mask.
        sigdelset(&blockMask, SIGTERM); // Remove SIGTERM from the mask.
        if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1) // Could we set the mask?
            errExit("sigprocmask");     // No, that's an error.
        printf("%s: signals blocked - sleeping %s seconds.\n", argv[0], argv[1]);
        sleep(getInt(argv[1], GN_GT_0, "block-time")); // Sleep for the block time.
        if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1) // Could we set the mask?
            errExit("sigprocmask");     // No, that's an error.
    }                                   // End of block time.
    sig = (argc > 1) ? getInt(argv[1], 0, "sig-num") : SIGTERM;
    printf("%s: PID is %ld.\n", argv[0], (long) getpid());

    while (!allDone)                    // While we're not done.
      pause();                          // Pause until a signal is caught.
    printf("Caught %d signals.\n", sigCount); // Report the number of signals caught.
    exit(EXIT_SUCCESS);                 // Exit successfully.
}
