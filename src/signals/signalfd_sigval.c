/**
 * This program demonstrates the use of signalfd(). This program creates a mask of the signal
 * numbers specified in its cmd-line arguments, blocks those signals, and then creates a signalfd
 * file descriptor to read those signals. It then loops, reading signals from the file descriptor
 * and displaying some of the information from the returned signalfd_siginfo structure. The program
 * is used in combination with t_siqueue.c to send realtime signal with accompanying data to the program.
 */
#include <sys/signalfd.h>
#include <signal.h>
//#include <linux/signal.h>
#include "tlpi_hdr.h"

int main(
    int argc,                          // The argument count.
    char* argv[])                      // The variable argument list.
{
    sigset_t mask;                    // The signal mask.
    int sfd;                          // The signal file descriptor.
    int j;                            // The loop counter.
    struct signalfd_siginfo fdsi;     // The signal file descriptor signal info.
    ssize_t nBytes;                   // The number of bytes read.

    if (argc <2 || strcmp(argv[1], "--help") == 0) // Need help?
      usageErr("%s sig-num...\n", argv[0]); // Yes, print help.
    printf("%s: PID is %ld.\n", argv[0], (long) getpid()); // Print the PID.
    sigemptyset(&mask);               // Clear the signal mask.
    for (j = 1; j < argc; j++)        // For all arguments.
      sigaddset(&mask, atoi(argv[j])); // Add the signal to the mask.
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) // Block the signals.
      errExit("sigprocmask");
    sfd = signalfd(-1, &mask, 0);     // Create the signal file descriptor.
    if (sfd == -1)                    // Did we get an error?
      errExit("signalfd");            // Yes, handle the error.
    for (;;)
    {
        nBytes=read(sfd, &fdsi, sizeof(struct signalfd_siginfo)); // Read the signal.
        if (nBytes != sizeof(struct signalfd_siginfo)) // Did we get an error?
          errExit("read");             // Yes, handle the error.
        printf("%s: got signal %d", argv[0], fdsi.ssi_signo); // Print the signal.
        if (fdsi.ssi_code == SI_QUEUE) // Is this a queued signal?
        {
            printf("; ssi_pid=%d; ", fdsi.ssi_pid); // Yes, print the PID.
            printf("ssi_int = %d.", fdsi.ssi_int); // Print the signal value.
        }
        printf("\n");
    }
}