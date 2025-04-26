/**
 * This program creates multiple child processes, one per (integer) command-line argument.
 * Each child sleeps for the number of seconds specified in the command-line argument, and then
 * exits. In the meantime, after all children have been created, the parent process repeatedly calls
 * wait() to monitor the termination of its children. This loop continues until wait returns -1. (This is
 * not the only approach: we could alternatively exit the loop when the number of children, numDead,
 * mathces the number of children created.)
 */
#include <sys/wait.h>                   // waitpid(), WNOHANG
#include <time.h>                       // time(), clock_gettime(), struct timespec
#include "curr_time.h"                  // currTime()
#include "tlpi_hdr.h"                   // error message macros and helpers

int main(
    int argc,                           // Number of command-line arguments
    char* argv[])                       // Command-line arguments
{
    int nDead;                          // Number of dead children.
    pid_t childPid;                     // PID of child process
    int j;                              // Loop counter
    if (argc < 2 || strcmp(argv[1], "--help") == 0) // If no args, or help needed.
      usageErr("%s sleep-time...\n", argv[0]); // Print usage message.
    setbuf(stdout, NULL);               // Disable buffering of stdout.
    for (j =1; j < argc; j++)           // Loop thru all cmd-line args.
    {
        switch(fork())                  // Fork into a new process.
        {                               // Handle all the child processes.
          case -1:                      // Failed to fork?
            errExit("fork");            // Print an error message.
          case 0:                       // Child process, sleeps then exit.
            printf("[%s] child %d started with PID %ld, sleeping %s seconds.\n",
              currTime("%T"), j, (long) getpid(), argv[j]); // Print a message.
            sleep(getInt(argv[j], GN_NONNEG, "sleep-time")); // Sleep for the specified time.
            _exit(EXIT_SUCCESS);        // Exit the child process.
          default:                      // Parent process.
            break;                      // Continue to the next child, by looping.
        }                               // End of switch statement.
    }                                   // End of for loop.
    nDead=0;                            // Initialize the number of dead children.
    for (;;)                            // Loop forever.
    {
        childPid = wait(NULL);          // Wait for a child to terminate.
        if (childPid == -1)             // Did we get an error?
        {                               // Yes, what type of error?
          if (errno == ECHILD)          // No children left to wait for?
          {                             // Yes, print a message.
            printf("No more children - bye!\n"); // Print a message.
            exit(EXIT_SUCCESS);         // Exit the program.
          }                             // End 
          else                          // Some other error?
          {                             // Yes, print an error message.
            errExit("wait");            // Print an error message.
          }                             // End of else statement.
        }                               // End of if statement.
        nDead++;                        // Increment the number of dead children.
        printf("[%s] wait() returned child PID %ld (numDead=%d)\n",
          currTime("%T"), (long) childPid, nDead); // Print a message.
    }
}