/**
 * This program creates one child process for each of its cmd-line args (2).
 * If the corresponding cmd line args is the letter d, then the child detaches
 * and places itself in its own process group. (3).; otherwise, it remains
 * in the same proc group as the parent. (We is the letter s to specify the
 * latter action, although any letter other than d will do). Each child
 * establishes a handler for SIGHUP (4). To ensure that they terminate
 * if no event occurs that would otherwise terminate them, the parent
 * and the children both call alarm() to set a timer that delivers
 * a SIGALRM after 60 seconds. (5). Finally, all procs (including the
 * parent) print out their PID, and PGID (6), and then loop waiting
 * for signals to arrive (7). When a signal is delivered, the handler
 * prints the process ID of the process and signal number (1).
 */
#define _GNU_SOURCE                   // Get strsignal from <string.h>
#include <string.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void Handler (                   // Our signal handler.
  int sig)                              // The signal to handle.
{                                       // ----------- Handler ----------------
  // (1) Print a message when the signal is received.
  // this is unsafe because printf() is not async-signal-safe.
  printf("PID=%ld: Caught signal %2d (%s).\n",
    (long)getpid(),sig,strsignal(sig));
}                                       // ----------- Handler ----------------

int main (                              // Our main function.
  int argc,                             // # of cmd line args.
  char* argv[])                         // List of cmd line args.
{                                       // ----------- main -------------------
  pid_t ppid;                           // The parent process ID.
  pid_t cpid;                           // The child process ID.
  struct sigaction sa;                  // The signal action structure.
  if (argc < 2 || strcmp(argv[1],"--help") == 0)
    usageErr("%s {d|s}... [ > sig.log 2>&1 ].\n",
      argv[0]);                         // Print usage message.
  setbuf(stdout,NULL);                  // Make stdout unbuffered.
  ppid=getpid();                        // Get the parent process ID.
  printf("PID of parent process: %ld.\n",(long)ppid);
  printf("Foreground process group id: %ld.\n",(long)tcgetpgrp(STDIN_FILENO));
  // (2) Create a child process for each cmd line arg.
  for (int j=1;j<argc;j++)              // For each cmd line arg...
  {                                     // Handle accordingly.
    cpid=fork();                        // Create a child process.
    if (cpid==-1)                       // Could we create a child?
      errExit("fork");                  // No, exit with error.
    // (3) If the child is to detach, and move to a new proc group
    // handle that here.                // 
    if (cpid==0)                        // Are we the child proc?
    {                                   // Yes, we are.
      if (argv[j][0]=='d')              // First char of argv[j] is d?
        if (setpgid(0,0)==-1)           // Can we move the child to new proc group?
          errExit("setpgid");           // No, exit with error.
      sigemtpyset(&sa.sa_mask);         // Clear the signal mask.
      sa.sa_flags=0;                    // No flags.
      sa.sa_handler=Handler;            // Set the signal handler.
      // (4) Set the signal action disposition for SIGHUP.
      if (sigaction(SIGHUP,&sa,NULL)==-1) // Set the signal action for SIGHUP.
        errExit("sigaction");           // Exit if error.
      break;                            // Exit the for loop.
    }                                   // Done if we created children.
  }                                     // Done handling arguments.
  // All processes fall through here.   //
  // (5) Set a timer to deliver a SIGALRM after 60 seconds.
  alarm(60);                            // Set a 60 sec timer to fire SIGALRM.
  // (6) Print out the PID and PGID.    //
  int len=printf("PID=%ld, PGID=%ld.\n",(long)getpid(),(long)getpgrp());
  if (len<0)                            // Check for error in printf.
    errExit("printf");                  // Yes, exit with error.
  for (;;)                              // (7) forever wait for signals to arrive.
    pause();                            // Wait for a signal.
}