/**
 * This program demonstrates the establishment and use of an alternate signal
 * stack. After establishing an alternate signal stack, the program sets a
 * handler for SIGSEGV, this program recurses infinitely, causing a stack
 * overflow and the process is sent a SIGSEGV signal.
 */
#define _GNU_SOURCE                     // Get strsignal() declaration from <string.h>
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void handler (                    // Function to handle Event signal generated.
    int sig)                             // The signal number.
{
    int top;                             // The top of the signal stack.

    // UNSAFE: This handler uses non-async-signal-safe functions (printf(),
    // strsignal(), fflush(); see Section 21.1.2)
    printf("Caught signal %d (%s).\n", sig, strsignal(sig));
    printf("Top of handler stack near    %10p\n", (void*)&top);
    fflush(NULL);                      // Flush all output streams.
    _exit(EXIT_FAILURE);               // No return after SIGSEGV.
}

// A recursive function that overflows the stack.
static void overflowStack(
    int callNum)                       // The number of the call.
{
    char nBytes[100000];               // # of bytes to allocate for stack.
    printf("Call %4d - top of the stack near %10p.\n", callNum, &nBytes[0]);
    overflowStack(callNum+1);          // Recurse.
}

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    stack_t sigstack;                   // The signal stack structure.
    struct sigaction sa;                // The signal action structure.
    int callNum;                        // The number of the call.
    printf("Top of standard stack is near %10p\n", (void*)&callNum);
    // -------------------------------- //
    // Allocate alternate stack and inform kernel of its existence.
    // -------------------------------- //
    sigstack.ss_sp = malloc(SIGSTKSZ);  // Allocate the stack.
    if (sigstack.ss_sp == NULL)         // Could we allocate the stack?
        errExit("malloc");              // No, exit.
    sigstack.ss_size = SIGSTKSZ;        // Set the size of the stack.
    sigstack.ss_flags = 0;              // No flags set.
    if (sigaltstack(&sigstack, NULL) == -1) // Could we set the alternate stack?
        errExit("sigaltstack");         // No, exit.
    printf("Alternate stack is at      %10p-%p\n", sigstack.ss_sp,
        (char*)sbrk(0)-1);              // Report the stack.
    sa.sa_handler=handler;              // Establish the signal handler.
    sa.sa_flags = SA_ONSTACK;           // Hanlder uses alternate stack.
    if (sigaction(SIGSEGV, &sa, NULL) == -1) // Could we set the signal action?
        errExit("sigaction");           // No, that's an error.
    overflowStack(1);                   // Start the stack overflow.  
    
}