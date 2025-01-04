/**
  Send a number of signals, or set of concurrent different signals to a PID
*/

#include <signal.h>
#include "tlpi_hdr.h"

int main (
  int argc,
  char* argv[])
{
  int nSigs;                            // The num of signals to send
  int sig;                              // Signal number to send.
  int j;                                // The counter.
  pid_t pid;                            // The process ID to sned signals to.
  
  // User wants help?
  if (argc < 4 || strcmp(argv[1], "--help"))
    usageErr("%s pid num-sigs sig-num [sig-num-2].\n",argv[0]);
  pid = getLong(argv[1], 0, "PID");     // Get the process to signal to.
  nSigs=getInt(argv[2], 0, "sig-nums"); // The # of signals to send.
  sig = getInt(argv[3], 0, "sig-num");  // The first signal to send.
  
  // ---------------------------------- //
  // Send signals to the receiver.
  // ---------------------------------- //
  printf("%s:  sending signal %d to process %ld, %d times.\n",
    argv[0], sig, (long) pid, nSigs);
  for (j =0; j < nSigs; j++)
    if (kill(pid, sig) == -1)           // Can't kill it?
      errExit("kill");                  // No, so exit.
      
  // If a fourth cmd-line arg was specified, send that signal.
  if (argc > 4)
    if (kill(pid, getInt(argv[4], 0, "sig-num-2")) == -1)
      errExit("kill");                  // Can't kill so exit.
  printf("%s: exiting.\n", argv[0]);
  exit(EXIT_SUCCESS);                   // If we got here, success.
}
