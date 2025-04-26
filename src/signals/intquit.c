/** Another signal hanndler example to display the use of managing different
 ** signals that could be generated which is how a signal handler should be 
 ** designed in the first place. 
*/
#include <signal.h>
#include "tlpi_hdr.h"

static void sigHandler(                 // Handle the received signal number.  
  int sig)                              // The received signal number.
{                                       // --------- sigHandler ------------- //
  static int count=0;                   // Number of times SIGINT was called.
  /*UNSAFE: This handler uses non-async-signal-safe functions
   (printf(),exit(); see section 21.1.2)*/
  if (sig == SIGINT)                    // Did we receive an Interrupt?
  {                                     // Yes, so let's handle it.
    count++;                            // Count the number of Interrupts.
    printf("Caught SIGINT (%d)\n",count);// Notify it (unsafe).
    return;                             // Return to main stack frame.
  }                                     // Done handling interrupt.     
  // Otherwise, must be a SIGQUIT - print a message and terminate
  // the process.
  printf("Caught SIGQUIT - Bye bye!.\n");
  exit(EXIT_SUCCESS);                   // If we got here we're good.  
}                                       // --------- sigHandler ------------- //

int main (                              // Our main program.
  int argc,                             // The number of arguments
  char* argv[])                         // The argument list in-itself.
{                                       // ----------- main ----------------- // 
  /* Establish same handler for SIGNINT and SIGQUIT. */
  if (signal(SIGINT, sigHandler) == SIG_ERR)// Did this signal cause an error ?
    errExit("signal");                  // Yes, exit.
  if (signal(SIGQUIT, sigHandler) == SIG_ERR)// Did this signal cause an error ?
    errExit("signal");                  // Yes, exit.
  for (;;)                              // Wait forever waiting for signals.
    pause();                            // Block 4ever until a signal is caught.
} 
