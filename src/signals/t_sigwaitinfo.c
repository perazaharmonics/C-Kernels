/**
 * This program first blocks all signals, then delays for the number of seconds specified in its
 * optional cmd-line argument. This allows signals to be sent to the program before sigwaitinfo().
 * The program then loops continuously using sigwaitinfo() to accept incoming signals, until SIGINT
 * or SIGTERM is received. Eventually the program completes its sleep interval, and the sigwaitinfo()
 * loop accepts the queued signals. As with realtime signal caught with a handler, we see that signals
 * are delivered lowest-numbered signal first, and that the siginfo_t structure passed to the signal
 * handler allows us to obtain the process ID and user ID of the sending process.
 */

#define _GNU_SOURCE
#include <string.h>
#include <signal.h>
#include <time.h>
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    int sig;                            // The signal number.
    siginfo_t si;                       // The signal information.
    sigset_t allSigs;                   // The signal set to block.
    if (argc > 1 && strcmp(argv[1], "--help") == 0) // User wants help?
      usageErr("%s [delay-secs].\n", argv[0]); // Print help.
    printf("%s: PID is %ld.\n", argv[0], (long) getpid()); // Print the PID.
    sigfillset(&allSigs);               // Fill the signal set.
    if (sigprocmask(SIG_BLOCK, &allSigs, NULL) == -1)// Block all signals.
      errExit("sigprocmask"); 
    printf("%s: signals blocked.\n", argv[0]); // Print the message.
    if (argc > 1)                       // Did we specify a delay?
    {                                   // Yes, let's handle that.
      printf("%s: about to delay %s seconds.\n", argv[0], argv[1]);
      sleep(getInt(argv[1], GN_GT_0, "delay-secs")); // Sleep for the delay.
      printf("%s: delay complete.\n", argv[0]);
    }
    for (;;)                            // Fetch signals until SIGINT or SIGTERM
    {
      sig = sigwaitinfo(&allSigs, &si); // Wait for signals, get signal info.
      if (sig == -1)                    // Got an error waiting for signals?
        errExit("sigwaitinfo");         // Yes, handle the error.
      if (sig == SIGINT || sig == SIGTERM) // SIGINT or SIGTERM?
        exit(EXIT_SUCCESS);             // Yes, exit.
      printf("Got signal: %d (%s).\n", sig, strsignal(sig)); // Print the signal.
      printf("    si_siginfo=%d, si_code=%d (%s), si_value=%d\n",
        si.si_signo, si.si_code,
        (si.si_code == SI_USER) ? "SI_USER" : 
        (si.si_code == SI_QUEUE) ? "SI_QUEUE" : "other",
        si.si_value.sival_int);
      printf("    si_pid=%ld, si_uid=%ld.\n", (long) si.si_pid, (long) si.si_uid);
    }
}