// This simple program creates another thread and then joins.
#include <pthread.h>
#include "tlpi_hdr.h"

static void* threadFunc(                // Start function for thread.
    void* arg)                          // Argument passed to the thread.
{                                       // ------ threadFunc() ------
    char* s=arg;                        // Cast arg to a string.
    printf("%s", s);                    // Print the string.
    return (void*) strlen(s);           // Return the length of the string.
}                                       // ------ threadFunc() ------

int main(
    int argc,                          // Number of cmd-line arguments.
    char* argv[])                      // Array of cmd-line arguments.
{                                      // ----------- main() ------------------
  pthread_t t1;                        // Thread handle.
  void* res;                           // Return value from thread.
  int s;                               // Status code.
  s=pthread_create(&t1, NULL, threadFunc, "Hello world!\n"); // Create a thread.
  if (s != 0)                          // Did pthread_create() fail?
    errExitEN(s, "pthread_create");    // Yes, print an error message.
  printf("Message from main()\n");     // Print a message.
  s=pthread_join(t1, &res);            // Wait for the thread to terminate.
  if (s != 0)                          // Did pthread_join() fail?
    errExitEN(s, "pthread_join");      // Yes, print an error message.
  printf("Thread returned %ld\n", (long) res); // Print the return value.
  exit(EXIT_SUCCESS);                  // Exit and indicate success.
}