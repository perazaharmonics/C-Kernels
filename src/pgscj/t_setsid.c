/**
 * This program demonstrates the use of setsid() to create a new session. To check
 * that it no longer has a controlling terminal, this program attempts to open the
 * special file /dev/tty. When looking at the output it can be seen that the process
 * successfully places itself in a new process group within a new session. Since
 * this session has no controlling terminal, the open() call fails. 
 */
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main (
    int argc,                           // Number of cmd line args.
    char* argv[])                       // List of cmd line args.
{                                       // ----------- main -------------------
  if (fork()!=0)                        // Exit if parent or on error.
    _exit(EXIT_SUCCESS);                // Parent process exits.
  if (setsid() == -1)                   // Can we create a new session?
    errExit("setsid");                  // No, exit with error.
  int len=printf("PID=%ld, PGID=%ld,SID=%ld.\n",
    (long)getpid(),(long)getpgrp(),(long)getsid(0));
  if (len<0)                            // Check for error in printf.
    errExit("printf");                  // Yes, exit with error.
  int fd0=open("/dev/tty",O_RDWR);      // Try to open /dev/tty.
  if (fd0!=0)                           // Could we open /dev/tty?
    errExit("open /dev/tty");           // no, exit with error.
  exit(EXIT_SUCCESS);                   // Yes, exit successfully.
}                                       // ----------- main -------------------