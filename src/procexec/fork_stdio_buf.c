/**
 * This program demonstrates the issue of how write() system call is buffered by the kernel
 * versus the buffering done by the stdio buffers. The write() system call is immediately
 * written to the kernel buffer, but the kernel buffers are flushed only when the buffer is full
 * or when the program terminates. The stdio buffers are flushed when the buffer is full, when
 * the program terminates, or when the fflush() function is called. The duplication of the stdio
 * buffers can cause the output to be duplicated, as shown in the output of this program, if not 
 * immediately flushed by the parent process before forking.
 */
#include <stdio.h>                      // printf(), fflush(), perror()
#include <stdlib.h>                     // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>                     // fork(), write(), close()
#include "tlpi_hdr.h"                   // error message macros and helpers

int main(
    int argc,                           // The number of cmd-line arguments
    char* argv[])                       // The cmd-line arguments
{
    printf("Hello World!\n");           // Print a message to stdout
    write(STDOUT_FILENO, "Ciao Mondo!\n", 12); // Write a message to stdout
    if (fork() == -1)                   // Fork into a offspring process
      errExit("fork");                  // Print an error message
    // -------------------------------- //
    // Both child and parent processes continue execution here.
    // -------------------------------- //
    exit(EXIT_SUCCESS);                 // Terminate the program

}