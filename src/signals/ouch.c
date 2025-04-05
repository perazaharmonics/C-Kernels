#include <signal.h>
#include "tlpi_hdr.h"

/** Example of a simple signal handler function and a main program that
*  establishes it as the handler for the SIGINT signal. The terminal driver
*  generates the signal when we type the terminal interrupt signal Ctrl+C
*  The handler simply prints the message and returns to main.
*/

static void SigHandler (int sig)        // The signal number to act upon
{
  printf("Ouch!\n");                    // Unsafe, see section 21.1.2
}

int main (
  int argc,                             // The number of input arguments.
  char* argv[])                         // The input argument list.
{
  int j;                                // The loop variable counter.
  if (signal(SIGINT, SigHandler) == SIG_ERR)// Could we handle the signa?
    errExit("signal");                  // No, that's an error.
  for (j=0; ; j++)                      // Loop until we get a SIGINT.
  {
    printf("%d\n",j);                   // Print the counter
    sleep(3);                           // Sleep a little you're too fast.
  }
}
