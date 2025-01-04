/**
 * Using sigqueue() to send real-time signals to a process. This program takes up to four
 * args of which the first three are mandatory. The first arg is the PID of the receiving
 * process, the second is the signal number to send, and the third is the data to send with
 * the signal. The fourth arg is optional and specifies the number of signals to send. 
 * If more than one signal is to be sent, the data value is incremented by one for each signal.
 * If this arg is not supplied, the program sends a single signal. The program sends the signals
 * using sigqueue() and the sigval union to pass the data. The receiving process can obtain
 * the data using the si_value field of the siginfo_t structure passed to the signal handler.
 */
#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    int sig;                            // The signal number.
    int numSigs;                        // The number of signals to send.
    int j;                              // The index of the signal.
    int sigData;                        // The signal data.
    union sigval sv;                    // The signal value.
    if (argc < 4 || strcmp(argv[1], "--help") == 0) // Not enough args?
      usageErr("%s pid sig-num data [num-sigs]\n", argv[0]); // Print help.
    
    // -------------------------------- //
    // Display our PID and UID, so that they can be compared
    // with the corresponding fields of the siginfo_t argument
    // supplied to the handler in the receiving process.
    // -------------------------------- //
    printf("%s: PID is %ld, UID is %ld.\n",
      argv[0], (long) getpid(), (long) getuid());
    sig = getInt(argv[2], 0, "sig-num"); // Get the signal number.
    sigData = getInt(argv[3], GN_ANY_BASE, "data"); // Get the signal data.
    numSigs = (argc > 4) ? getInt(argv[4], GN_GT_0, "num-sigs") : 1; // Get the number of signals.
    // -------------------------------- //
    // Send the specified number of signals to the specified process.
    // -------------------------------- //
    for (j = 0; j<numSigs; j++)         // For the number of signals to send.
    {
      sv.sival_int = sigData + j;       // Set the signal value for this signal.
      if (sigqueue(getLong(argv[1], 0, "pid"), sig, sv) == -1) // Could we send the signal?
        errExit("sigqueue %d", j);      // No, that's an error.
    }
    exit(EXIT_SUCCESS);                 // Exit successfully.
}