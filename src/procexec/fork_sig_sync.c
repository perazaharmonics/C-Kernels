/**
 * This program demonstrates how to synchronize parent and child processes using signals.
 * We assume that the parent must waint on the child to carry some action. The signal-related
 * call in the parent can be swapped if the child must wait on the parent. It is even possible
 * for both parent and child to signal each other multiple times in order to coordinate their actions.
 * Although in practice, this likely done using semaphores, file locks, and sending messages between
 * processes using pipes.
 */

#include <signal.h>                     // SIGUSR1, SIGUSR2, sigset_t, sigemptyset, sigaddset, sigprocmask, sigaction, sigsuspend
#include "curr_time.h"                  // currTime declaration()
#include "tlpi_hdr.h"                   // error message macros and helpers 

#define SYNC_SIG SIGUSR1                // Signal used for synchronization
 
static void handler(int sig)            // Signal handler - does nothing but return
{
    // Do nothing
}

int main(
    int argc,                           // Number of command-line arguments
    char* argv[])                       // Command-line arguments variable list.
{
  pid_t childPid;                       // PID of child process
  sigset_t bMask;                       // Signal mask to block SIGUSR1
  sigset_t oMask;                       // Signal mask to save the current signal mask
  sigset_t emptyMask;                   // Signal mask to unblock SIGUSR1
  struct sigaction sa;                  // Signal handler specification
  setbuf(stdout, NULL);                 // Disable buffering of stdout.
  sigemptyset(&bMask);                  // Initialize signal mask
  sigaddset(&bMask, SYNC_SIG);          // Add SYNC_SIG to signal block mask
  // Block SYNC_SIG, and save the current signal mask.
  if (sigprocmask(SIG_BLOCK, &bMask, &oMask) == -1) // Block SYNC_SIG
    errExit("sigprocmask");
  sigemptyset(&sa.sa_mask);             // Initialize signal mask
  sa.sa_flags = SA_RESTART;             // Restart system calls if possible
  sa.sa_handler = handler;              // Set the signal handler
  if (sigaction(SYNC_SIG, &sa, NULL) == -1) // Try to set signal handler for SYNC_SIG
    errExit("sigaction");               // If it fails, print an error message
  switch(childPid = fork())             // Fork into a new process.
  {
    case -1:                            // Failed to fork?
      errExit("fork");                  // Print an error message.
    case 0:                             // Child process
      // Child does some action here
      printf("[%s %ld] Child labor has begun!\n",
        currTime("%T"), (long) getpid());
      sleep(2);                         // Simulate time doing labor.
      // Then signal parent that it's done.
      printtf("[%s %ld] Child about to signal parent.\n",
        currTime("%T"), (long) getpid());
      if (kill(getpid(), SYNC_SIG) == -1) // Could we signal the parent?
        errExit("kill");                  // No, print an error message.
      // Now child can do other things.
      _exit(EXIT_SUCCESS);               // Exit the child process.
    default:                             // Parent process.
      // Parent may do some work here and then wait for child to signal it,
      // after it is done with the child labor.
      printf("[%s %ld] Parent about to wait for signal.\n",
        currTime("%T"), (long) getpid());
      sigemptyset(&emptyMask);          // Initialize signal mask
      if (sigsuspend(&emptyMask) == -1 && errno != EINTR) // Wait for signal
        errExit("sigsuspend");           // Print an error message.
      printf("[%s %ld] Parent got signal.\n",
        currTime("%T"), (long) getpid());
      // If required, return the signal mask to its original state.
      if (sigprocmask(SIG_SETMASK, &oMask, NULL) == -1)
        errExit("sigprocmask");
      // Parent can now do other things.
      exit(EXIT_SUCCESS);                // Exit the parent process.
  }
}