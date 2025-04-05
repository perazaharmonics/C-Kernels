/**
 * This program demonstrates the creation of a zombie process and that a zombie process
 * can't be killed by SIGKILL. The program uses the system() function to execute the shell
 * command given in its character string argument.
 */
#include <signal.h>
#include <libgen.h>
#include "tlpi_hdr.h"

#define CMD_SIZE 100

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The var argument list.
{
    char cmd[CMD_SIZE];                 // The cmd to execute.
    pid_t childPid;                     // Child process ID
    setbuf(stdout, NULL);               // Disable stdout buffering.
    printf("Parent PID=%ld", (long) getpid());
    switch (childPid = fork())
    {
        case -1:
          errExit("fork");
        case 0:                         // Child immediately exit to become zombie
          printf("Child (PID=%ld) exiting.\n", (long) getpid());
          _exit(EXIT_SUCCESS);
        default:                        // Parent
          sleep(3);                     // Give child a chance to start and exit.
          snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
          system(cmd);                   // View zombie child.
          // Now send the SIGKILL signal to the zombie child.
          if (kill(childPid, SIGKILL) == -1)
            errMsg("kill");              // Failed to kill the child.
          sleep(3);                      // Give child a chance to react to signal.
          printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);
          system(cmd);                   // View zombie child again.
          exit(EXIT_SUCCESS);            // Exits the parent process.
    }
}