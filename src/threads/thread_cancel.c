/**
 * Simple example of the using of pthread_cancel(). The main program
 * creates a thread that executres an infinite loop, sleeping for a second
 * and printing the value of a loop counter. (This thread will terminate only
 * if it is sent a cancellation request or if the process exits) Meanwhile,
 * the main thread sleeps for 3 seconds and then sends a cancellation request
 * to the thread that it created.
 */
#include <pthread.h>
#include "tlpi_hdr.h"

static void* threadFunc (void* arg)
{
    printf("New thread started.\n");    // May be used as cancellation point.
    for (int j=1; ; j++)   
    {
      printf("Loop: %d.\n", j);         // May be used as cancellation point.
      sleep(1);                         // Sleep for 1 second. (cancellation point)
    }
    return NULL;                       // NOT REACHED.
}

int main (
    int argc,
    char* argv[])
{
    pthread_t tid=0;                      // The thread ID.
    int s=0;                              // Status code.
    void* res=(void*) 0;                  // The return value of the thread.
    s=pthread_create(&tid,NULL,threadFunc,NULL); // Create the thread.
    if (s!=0)                             // Did we fail to create the thread?
        errExitEN(s,"pthread_create");    // Yes, print error and exit.
    sleep(3);                             // Allow new thread to sleep a while.
    printf("Cancelling thread.\n");       // Print message.
    s=pthread_cancel(tid);                // Send cancellation request to the thread.
    if (s!=0)                             // Did we fail to send the cancellation request?
        errExitEN(s,"pthread_cancel");    // Yes, print error and exit.
    s=pthread_join(tid,&res);             // Wait for the thread to terminate.
    if (s!=0)                             // Did we fail to join the thread?
        errExitEN(s,"pthread_join");      // Yes, print error and exit.
    if (res==PTHREAD_CANCELED)            // Did the thread terminate?
      printf("Thread was cancelled.\n");  // Yes, print message.
    else                                  // No, the thread terminated normally.
      printf("The thread was not cancelled (should not happen!).\n");
    exit(EXIT_SUCCESS);                   // Exit with success.
}