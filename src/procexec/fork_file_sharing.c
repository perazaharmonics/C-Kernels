/**
 * This program demonstrates how file sharing works in forked processes. It opens
 * a temporary file using mkstemp(), and then call fork() to create a child process.
 * The child process changes the file offset and open file status flag of the tmp ile
 * and exit. The parent then retrieves the file offset and flags to verify that it can
 * see the changes made by the child.
 */
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The variable argument list.
{
    int fd;                             // The open file descriptor.
    int flags;                         // The file status flags.
    char template[]="/tmp/testXXXXXX"; // The template for the temporary file.
    setbuf(stdout, NULL);               // Disable buffering of stdout.
    fd = mkstemp(template);             // Create a temporary file.
    if (fd == -1)                       // Did we get an error?
      errExit("mkstemp");               // Yes, handle the error.
    printf("File offset before fork(): %lld.\n", 
      (long long) lseek(fd, 0, SEEK_CUR)); // Print the file offset.
    flags = fcntl(fd, F_GETFL);         // Get the file status flags.
    if (flags == -1)                    // Did we get an error?
      errExit("fcntl - F_GETFL");       // Yes, handle the error.
    printf("O_APPEND flag before fork() is: %s.\n",
      (flags & O_APPEND) ? "on" : "off"); // Print the O_APPEND flag.
    switch(fork())                      // Create a child process.
    {
        case -1:                         // Error forking?
          errExit("fork");               // Yes, handle the error.
        // ----------------------------- //
        // Child process changes file offset and status flags.
        // ----------------------------- //
        case 0:                          // Our child.
          if (lseek(fd, 1000, SEEK_SET) == -1) // Change the file offset.
            errExit("lseek");            // Handle the error.
          flags = fcntl(fd, F_GETFL);    // Get the file status flags.
          if (flags == -1)               // Did we get an error?
            errExit("fcntl - F_GETFL");  // Yes, handle the error.
          flags |= O_APPEND;             // Turn on the O_APPEND flag.
          if (fcntl(fd, F_SETFL, flags) == -1) // Set the file status flags.
            errExit("fcntl - F_SETFL");  // Handle the error.
          _exit(EXIT_SUCCESS);           // Exit the child.
        // ----------------------------- //
        // Parent process retrieves file offset and status flags.
        // ----------------------------- //
        default:                         // The parent.
          if (wait(NULL) == -1)          // Wait for the child to exit.
            errExit("wait");             // Handle the error.
          printf("Child has exited.\n"); // Print the message.
          printf("File offset in parent: %lld.\n",
            (long long) lseek(fd, 0, SEEK_CUR)); // Print the file offset.
          flags = fcntl(fd, F_GETFL);    // Get the file status flags.
          if (flags == -1)               // Did we get an error?
            errExit("fcntl - F_GETFL");  // Yes, handle the error.
          printf("O_APPEND flag in parent is: %s.\n",
            (flags & O_APPEND) ? "on" : "off"); // Print the O_APPEND flag.
          exit(EXIT_SUCCESS);            // Exit the program.
    }
}