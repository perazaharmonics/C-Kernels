// What possible outcomes can occur if a thread executes the following code?
// pthread_join(pthread_self(), NULL);
// A. Deadlock:
// The thread will deadlock. The thread will wait indefinitely for itself to terminate,
//    leading to deadlock. Since the thread cannot terminate until it joins with itself, it
//    will be stuck in an infinite state of waiting.
// B. Error Return:
// The pthread_join() function may return an error. According to POSIX standard, pthread_join(),
//    should return the error code EDEADLK if a deadlock is detected. However, not all implementations
//    may return this error code.
// C. Undefuned behavior:
// Since joining itself is not a valid operation, the behavior is undefined. This means
// the program may crash, exhibit unexpected behavior, or produce incorrect results.

#include <pthread.h>
#include "tlpi_hdr.h"

// Start function for thread (what the thread does).
static void* threadFunc(                // Start function for thread.
    void* arg)                          // Argument passed to the thread.
{                                       // ------ threadFunc() ------
    char* s=arg;                        // Cast arg to a string.
    printf("%s", s);                    // Print the string.
    return (void*) strlen(s);           // Return the length of the string.
}                                       // ------ threadFunc() ------

// Write a program that creates a thread and tries to prevent it from joining
// with itself. 
int main(
    int argc,                           // Number of cmd-line arguments.
    char* argv[])                       // Array of cmd-line arguments.
{                                       // ----------- main() ------------------
  pthread_t t1;                         // Thread handle.
  void* res;                            // Return value from thread.
  int s;                                // Status code.
  s=pthread_create(&t1, NULL,threadFunc,"Hello world!\n"); // Create a thread.
  if (s != 0)                           // Did pthread_create() fail?
    errExitEN(s, "pthread_create");     // Yes, print the error msg.
  printf("Message from main()\n");      // Print a message.
  if (pthread_self() != t1)             // Is the thread not joining itself?
  {                                     // Yes, it's safe to join the thread.
    s=pthread_join(t1, &res);           // Wait for the thread to terminate.
    if (s != 0)                         // Failed to join the thread?
      errExitEN(s, "pthread_join");     // Yes, print an error msg.
  }                                     // Done trying to prevent deadlocks, etc...
  else                                  // The thread is trying to join itself.
    printf("Thread is tring to join itself.\n"); // Print a message.
  exit(EXIT_SUCCESS);                   // Exit and indicate success.
}                                       // ----------- main() ------------------