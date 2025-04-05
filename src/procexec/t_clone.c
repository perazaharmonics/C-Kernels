/**
 * This program demonstrates the use of clone() system call to create a child process.
 * It does the following: 
 * (2) Open a file descriptor (for /dev/null) in the parent process.
 * Set the value for the clone() flags argument to CLONE_FILES (3) if a cmd-line arg was
 * supplied, so that the parent and child will share a single file descriptor table. If
 * no cmd line argument was supplied, flags is set to 0.
 * (4) Allocate a stack for use by the child.
 * If CHILD_SIG is nonzero and not equal to SIG_CHLD, ignore it, in case it is a signal
 * that would terminate the process (5). We don't ignore SIGCHLD, because doing so
 * would prevent waitaing on the child to collect its status. 
 * (6) Call clon() to create the child. The third (bit-mask) argument includes the 
 * termination signal. The fourth argument (func_arg) specifies the file descriptor
 * opened earlier (at (2)).
 * (7) Wait for the child to terminate.
 * Check whether fd is still open, by trying to write() to it (8). The program
 * reports whether the write() succeeds or fail.
 */
#define _GNU_SOURCE
#include <signal.h>                      // For signal()
#include <sys/wait.h>
#include <fcntl.h>                       // For open()
#include <sched.h>                       // For clone()
#include "tlpi_hdr.h"                    // For error messages

#ifndef CHILD_SIG
#define CHILD_SIG SIGUSR1
#endif

/* (1) */
static int childFunc(void* arg)
{
    if (close(*((int *) arg)) == -1)// Could we close the file descriptor?
        errExit("close");            // No, print an error message.
    return 0;                        // Return 0.
}

int main(
    int argc,                           // Number of cmd-line arguments.
    char* argv[])                       // Array of cmd-line arguments.
{
    const int STACK_SIZE = 65536;       // Stack size for child.
    char* stack;                        // Start of stack buffer.
    char* stackTop;                     // End of stack buffer.
    int s;                              // Status returned by waitpid().
    int fd0;                            // File descriptor for /dev/null.
    int flags;                          // Flags for clone() call.
/* (2) */    
    fd0=open("/dev/null", O_RDWR);      // Open /dev/null.
    if (fd0 == -1)                      // Did open() fail?
        errExit("open");                // Yes, print an error message.
/* If argc > 1, child shares fd table with parent. */
/* (3) */
    flags=(argc>1) ? CLONE_FILES:0;     // Set flags for clone() call.
/* (4) */
    stack=malloc(STACK_SIZE);           // Allocate stack space fir child.
    if (stack == NULL)                  // Did malloc() fail?
        errExit("malloc");              // Yes, print an error message.
    stackTop=stack+STACK_SIZE;          // Calculate top of stack. Assume sack growth downward.
/* (5) */
    // -------------------------------- //
    // Ignore CHILD_SIG, in case it is a signal whose default is to terminate the process.
    // but dont ignore SIGCHLD (which is ignored by default), since that would prevent
    // the creation of a zombie.
    // -------------------------------- //
    if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD)
        if (signal(CHILD_SIG, SIG_IGN) == SIG_ERR)
            errExit("signal");
/* (6) Create a child; child commences execution in childFunc() */
    if (clone(childFunc, stackTop, flags | CHILD_SIG, (void*) &fd0) == -1)
        errExit("clone");              // Error creating child.
    // -------------------------------- //
    // Parent falls through here. Wait for child; _WCLONE is needed for child notifying
    // using signal other than SIGCHLD.
    // -------------------------------- //
/* (7) */
    if (waitpid(-1, NULL, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0) == -1)
        errExit("waitpid");
/* (8) */
    s = write(fd0, "x", 1);             // Try to write to fd.
    if (s == -1 && errno == EBADF)      // Did close fd in child affect parent?
        printf("File descriptor %d has been closed\n", fd0);
    else if (s == -1)                   // Did write() succeed?
        printf("write() on file descriptor %d failed unexpectedly (%s)\n", fd0, strerror(errno));
    else                               // write() succeeded.
        printf("write() on file descriptor %d succeeded\n", fd0);
    exit(EXIT_SUCCESS);                 // Exit the program.      
}