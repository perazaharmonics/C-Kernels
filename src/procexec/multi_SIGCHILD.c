/**
 * This program demonstrates an example of a more complex handler for a SIGCHLD signal.
 * This handler displays the PID and wait status of each reaped child(1) process.
 * In order to see that multiple child signals are not queued while the handler is
 * already invoked, execution of the handler is artifically delayed by a call to sleep(). (2).
 * The main program is creates one child process for each integer cmd line arugment (4).
 * Each of these children sleeps for the number of seconds specified in the corresponding
 * cmd-line argument before exiting (5). In the following example we see that even though
 * three children terminate, SIGCHILD is only queued twice to the parent. 
 * 
 * Note the use of sigprocmask() to block the SIGCHILD signal before any children are
 * created (3). This is done to ensure correct operation of the sigsuspend() loop in
 * the parent process. If we failed to block the SIGCHILD signal in this way, and a child
 * process terminated between the test of the value of nLiveChildren and the execution of
 * the sigsuspend() call (or alternatively, the pause() call), then the sigsuspend() call
 * would block forever waiting for a signal that has already been caught (6). The requirement
 * for dealing with such condition is detailed in Section 22.9 of The Linux Programming Interface.
 * 
 */
#include <signal.h>
#include <sys/wait.h>
#include "print_wait_status.h"
#include "curr_time.h"
#include "tlpi_hdr.h"

static volatile int nLiveChildren = 0;  // Number of children currently alive.

// Function to handle the SIGCHLD signal.
static void sigchldHandler(
    int sig)                            // The signal number to handle.
{
    int status;                         // The status of the child process.
    int savederr;                       // The error number saved by the kernel.
    pid_t childPid;                     // The child process ID.

    // -------------------------------- //
    // UNSAFE: This handler uses non-async-signal-safe functions
    // (printf(), printWaitStatus(), currTime() )
    // -------------------------------- //
    savederr = errno;                   // Save the error number.
    printf("%s handler: Caught SIGCHILD!.\n", currTime("%T"));
    // -------------------------------- //
    // (1) Reap any children that have already terminated.
    // -------------------------------- //
    while (                             // While we still have children to reap.
        (childPid = waitpid(-1, &status, WNOHANG)) > 0)
    {                                   // Reap the child.
        printf("%s handler: Reaped child %ld - ", currTime("%T"), (long) childPid);
        printWaitStatus(NULL, status);  // Print the status of the child.
        nLiveChildren--;                // Another child dead.
    }                                   // Done reaping dead children.
    if (childPid == -1 && errno != ECHILD)// Did we fail to reap a child?
      errMsg("waitpid");                // Yes, print an error message.
    // -------------------------------- //
    // (2) Artifficialy delay the execution of the handler to see that multiple
    //    child signals are not queued while the handler is already invoked.
    // -------------------------------- //
    sleep(5);                           // Delay handler execution.
    printf("%s handler returning.\n", currTime("%T"));
    errno = savederr;                   // Restore the error number.
}

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{                                       // ------------- main() --------------- //
    int j;                              // Loop counter.
    int sigCnt;                         // Number of signals received.
    sigset_t bMask;                     // The mask for signals to block.
    sigset_t eMask;                     // The mask for signals to unblock.
    struct sigaction sa;                // The signal action structure.

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
      usageErr("%s child-sleep-time...\n", argv[0]);
    setbuf(stdout, NULL);               // Disable stdout buffering.
    sigCnt = 0;                         // Initialize the signal count.
    nLiveChildren = argc - 1;           // Number of children to create.
    sigemtpyset(sa.sa_mask);            // Initialize the signal mask.
    sa.sa_flags = 0;                    // No flags.
    sa.sa_handler = sigchldHandler;     // Set the handler.
    if (sigaction(SIGCHLD, &sa, NULL) == -1)// Set the handler for SIGCHLD.
      errExit("sigaction");             // Exit if it failed.
    // -------------------------------- //
    // (3) Block SIGCHLD to prevent its delivery if a child terminates before the parent
    //     commences the sigsuspend() loop.
    // -------------------------------- //
    sigemtpyset(&bMask);                // Initialize the block mask.
    sigaddset(&bMask, SIGCHLD);         // Add SIGCHLD to the block mask.
    if (sigprocmask(SIG_SETMASK, &bMask, NULL) == -1)
        errExit("sigprocmask");         // Exit if it failed.
    // -------------------------------- //
    // (4) Create one child process for each cmd-line argument.
    // -------------------------------- //
    for (j =1; j < argc; j++)           // For the number of input arguments.
    {                                   // Create child processes.
      switch(fork())                    // Fork to create a child.
      {                                 // Switch on the fork result.
        case -1:                        // Fork failed.
          errExit("fork");              // Exit if fork failed.
        case 0:                         // Child process, sleeps and then exits.
          sleep(getInt(argv[j], GN_NONNEG, "child-sleep-time"));
          printf("%s Child %d (PID=%ld) exiting!\n", currTime("%T"),
            j, (long) getpid());        // Print the child exiting.
          _exit(EXIT_SUCCESS);          // Exit the child.
        default:                        // Parent process.
          break;                        // Continue to the next child.
      }                                 // Done handling the fork result.
    }                                   // Done creating children.
    // -------------------------------- //
    // (5) Parent process.
    // -------------------------------- //
    // Parent comes here: wait for SIGCHLD until all children are dead.
    sigemtpyset(&eMask);                // Initialize the empty mask.
    while (nLiveChildren > 0)           // While we still have children:
    {                                   // Wait for the children to die.
      if (sigsuspend(&eMask) == -1 && errno != EINTR)// Suspend until signal received.
        errExit("sigsuspend");          // Exit if it failed.
      sigCnt++;                         // Increment the signal count.
    }                                   // Done waiting for children to die.
    printf("%s All children have terminated; SIGCHLD was caught %d times.\n",
      currTime("%T"), sigCnt);          // Print the number of times SIGCHLD was caught.
    exit(EXIT_SUCCESS);                 // Exit the parent.
}                                       // ------------- main() --------------- //