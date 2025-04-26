/**
 * This program demonstrates that when the shell receives SIGHUP, it in turn sends
 * SIGHUP to the jobs it has created. The main task of this program is to create
 * a child process, and then have both the parent and child process pause to 
 * catch the SIGHUP signal and display a message if it is received. If the
 * program is given an optional argument (which may be any string), it will
 * display the child places itself in a different process group under the same
 * session. This is useful to show that the shell doesn't send SIGHUP to a 
 * process group that it did not create, even if it is in the same session
 * as the shell. (Since the final for loop of the program cycles forever,
 * this program uses alarm() to establish a timer to deliver SIGALRM. The
 * arrival of an unhandled signal guarantees process termination, if the 
 * process is not otherwise terminated)
 */
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <signal.h>
#include "tlpi_hdr.h"

static void Handler (                   // Our signal handler.
    int sig)                            // The signal to handle.
{                                       // ----------- Handler ----------------
  // Do something with the SIGHUP like restarting the terminal
  // that has the log file open.
  // This is a no-op handler, but we could do something useful here.
}                                       // ----------- Handler ----------------

int main (                              // Our main function.
  int argc,                             // Number of cmd line args.
  char* argv[])                         // List of cmd line args.
{                                       // ----------- main -------------------
  pid_t cpid;                           // The child process ID.
  struct sigaction sa;                  // The signal action structure.
  setbuf(stdout,NULL);                  // Make stdout unbuffered.
  sigemptyset(&sa.sa_mask);             // Clear the signal mask.
  sa.sa_flags=0;                        // No flags.
  sa.sa_handler=Handler;                // Set the signal handler.
  if (sigaction(SIGHUP,&sa,NULL)==-1)   // Set the signal actionS for SIGHUP.
    errExit("sigaction");               // Exit if error.
  cpid=fork();                          // Create a child process.
  if (cpid==-1)                         // Could we create a child?
    errExit("fork");                    // No, exit with error.
  if (cpid==0 && argc>1)                // We are the child and have an arg to handle?
    if (setpgid(0,0)==-1)               // Move the child to a new proc group.
      errExit("setpgid");               // Couldn't move to new proc group.
  int len=printf("PID=%ld, PGID=%ld, SID=%ld, Received: %s.\n",
        (long)getpid(),(long)getpgrp(),(long)getsid(0));
  if (len<0)                            // Did we write anything?
    errExit("printf");                  // No, exit with error.
  // ---------------------------------- //
  // Setting a 60 second timer ensure the process terminates
  // sets a timer to deliver a SIGALRM after 60 seconds.
  // Because we don't handle SIGALRM, this ensures the proc
  // terminates after 60 seconds.
  // ---------------------------------- //
  alarm(60);                            // Set a 60 second timer.
  for (;;)                              // Forever..
  {                                     //
    pause();                            // Wait for a signal.
    int len=printf("%ld: Caught SIGHUP.\n",(long)getpid());;
      if (len<0)                        // Check for error in printf.
        errExit("printf");              // Yes, exit with error.
  }
}