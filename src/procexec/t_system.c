/**
 * This program demonstrates the use of system() system call to execute a shell command.
 * This program executes a loop that reads a command string, executes it using system(),
 * and then analyzes and displays the value returned by system().
 */
#include <sys/wait.h>                    // For waitpid()
#include "print_wait_status.h"           // For printWaitStatus()
#include "tlpi_hdr.h"                    // For error messages

#define MAX_CMD_LEN 200

int main (
    int argc,                           // Number of cmd-line arguments.
    char* argv[])                       // Array of cmd-line args.
{
    char str[MAX_CMD_LEN];              // Buffer for cmd string.
    int status;                         // Status returned by system().
    for (;;)                            // Loop forever.
    {
        printf("Command: ");            // Prompt for a command.
        fflush(stdout);                 // Flush the output buffer.
        if (fgets(str, MAX_CMD_LEN, stdin) == NULL)// Get cmd from stdin, place into string.
            break;                      // If we get EOF, break out of the loop.
        status=system(str);             // Execute the cmd, get the status.
    // -------------------------------- //
    // Bit shifting status extracts the exit status of the cmd.
    // The exit status is stored in the higher-order byte of the status code.
    // By shifting the status code 8 bits to the right, we move the exit status
    // to the lower-order byte, making it easier to read.
    // The bit masking operation extracts the signal number that caused the cmd
    // to terminate, if it was terminated by a signal. The signal number is stored
    // in the lower-order byte of the status code. By performing the bitwise AND
    // with 0xFF, you mask out all but the lower-order byte, isolating the signal
    // number.
    // -------------------------------- //
        printf("system() returned: status=0x%04x (%d,%d)\n",
            (unsigned int) status, status >> 8, status&0xFF);
        if (status == -1)               // Did system() fail?
            errExit("system");          // Yes, print the error msg.
        else                            // Else, something else happened.
        {                               // So let's handle that.
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
                printf("(Probably) could not invoke shell...\n");
            else                        // Else, the cmd was executed.
                printWaitStatus(NULL, status);// Print the status.
        }                               // Done handling the status returned.
    }                                   // Done with the infinite loop.
    exit(EXIT_SUCCESS);                 // Exit the program.
}