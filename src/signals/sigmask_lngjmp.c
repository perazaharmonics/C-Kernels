/** 
 * This program demonstrates the difference in signal mask handling for the two
 * nonlocal gotos. It establishes a handler for the SIGINT signal. It is designed
 * to allow either setjmp() plus longjmp() or sigsetjmp() plus siglongjmp() to
 * be used to exit the signal handler, depending on whether the program is compiled
 * with the -DUSE_SIGSETJMP macro defined. The program displays the current settings of
 * the signal mask both on entry to the signal handler and after the non-local goto has
 * transferred control from the handler back to main(). 
 *  
*/
#define _GNU_SOURCE                     // Get strsignal() declaration from <string.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include "signal_functions.h"            // Declaration of printSigMask()
#include "tlpi_hdr.h"

static volatile sig_atomic_t canJump = 0;// Set to 1 once "env" buf has been
                                        //   initialized by [sig]setjmp()
#ifdef USE_SIGSETJMP
static sigjmp_buf senv;                 // Stores the environment saved by sigsetjmp()
#else
static jmp_buf env;                     // Stores the environment saved by setjmp()
#endif

static void handler(                   // Signal handler - for SIGINT
    int sig)                           // The signal number.
{
    // -------------------------------- //
    // UNSAFE: This handler uses non-async-signal-safe functions (printf(),
    // strsignal(); see Section 21.1.2)
    // -------------------------------- //
  printf("Received signal %d (%s), signal mask is:\n",
    sig, strsignal(sig));
  printSigMask(stdout, NULL);          // Print the signal mask.

  if (!canJump)
  {
    printf("env buffer not yet set, doing a simple return.\n");
    return;
  }
    // -------------------------------- //
    // Reset signal mask and unblock SIGINT if environment is set.
    // -------------------------------- //

#ifdef USE_SIGSETJMP
  siglongjmp(senv, 1);
#else
  longjmp(env, 1);
#endif
}

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    struct sigaction sa;                // The signal action structure to set.
    printSigMask(stdout, "Signal mask at startup:\n");
    sigemptyset(&sa.sa_mask);           // Clear the signal mask.
    sa.sa_flags = 0;                    // No flags set.
    sa.sa_handler = handler;            // The signal handler.
    if (sigaction(SIGINT, &sa, NULL) == -1)// Could we set the signal action?
      errExit("sigaction");             // No, that's an error.

     // ------------------------------- //
     // Set the jump buffer. (environment)
     // ------------------------------- //    
#ifdef USE_SIGSETJMP
  printf("Calling sigsetjmp().\n");     // Report the call.
  if (sigsetjmp(senv, 1) == 0)          // Set the environment.                      
#else
  printf("Calling setjmp().\n");        // Report the call.
  if (setjmp(env) == 0)                 // Set the environment.                      
#endif
    // -------------------------------- //
    // Executed after [sig]setjmp() establishes env.
    // -------------------------------- //
    canJump = 1;                        // Notify we can jump. 
  else                       
    printSigMask(stdout, "After jump from handler, signal mask is:\n");   

  
  for (;;)                              // Wait for signal until interrupted.
    pause();                            // Wait for a signal.
  
}

                                        