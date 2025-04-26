/**
 * This programs demonstrates the use of atexit() and on_exit() to register 
 * exit handlers that are called when the program terminates. The handlers
 * are called in the reverse order of their registration.
 */
#define _BSD_SOURCE
#include <stdlib.h>                     // EXIT_SUCCESS, EXIT_FAILURE, atexit(), on_exit()
#include "tlpi_hdr.h"                   // error message macros and helpers

static void atExitFunc1 (void)          // Called when exit() is called
{
    printf("atexit function 1 called\n");
}
static void atExitFunc2 (void)          // Called when exit() is called
{
    printf("atexit function 2 called\n");
}
static void onExitFunc (
    int status,                         // The exit status of the program.
    void* arg)                          // The argument passed to on_exit()
{
    printf("on_exit function called: status=%d,arg=%ld\n", status, (long) arg);
}

// The exit handlers will be called in the reverse order of their registration.
int main(
    int argc,                           // # of comd-line args
    char* argv[])                       // Command-line arguments
{
    if (on_exit(onExitFunc, (void*) 10) != 0) // Register onExitFunc
      fatal("on_exit 1");
    if (atexit(atExitFunc1) != 0)       // Register atexitFunc1
      fatal("atexit 1");
    if (atexit(atExitFunc2) != 0)       // Register atexitFunc2
      fatal("atexit 2");
    if (on_exit(onExitFunc, (void*) 20) != 0) // Register onExitFunc
      fatal("on_exit 2");

    exit(2);                            // Terminate the program
}