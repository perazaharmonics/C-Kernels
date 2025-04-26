/**
 * This program demonstrates how to join multiple threads in a program. The program
 * creates one thread for each of the comd-line arguments and then waits (sleeps) for
 * the number of seconds specified in the cmd line arguments, and then terminates.
 * The sleep interval if our way of simulating the idea of a thread doing some work
 * for a period of time. The program mantains a set of global variables recording
 * information about all threads that have been created. For each thread, an element
 * in the global thread array records the ID of the thread (the tid field) and its
 * current state (the state field). The state field has one of the following values.
 * TS_ALIVE: The thread is still running.
 * TS_TERMINATED: The thread has terminated but not yet been joined.
 * TS_JOINED: The thread has been terminated and joined.
 * 
 * As each thread terminates it assigns the value TS_TERMINATED to its state field.
 * for its element in the global thread array, increments a global counter of terminated
 * but as of yet unjoined threads (nUnjoined) and signals the conditions variable threadDied.
 * The main thread employs a loop that continously waits on the condition variable threadDied.
 * Whenever threadDied is signaled and there are terminated threads that have not been joined,
 * the main thread scans the thread array, looking for elements with state set to
 * TS_TERMINATED. For each thread in this state, pthread_join() is called to join the thread
 * using the corresponding tid field from the thread array and then the state is set to
 * TS_JOINED. The main loop terminates when all of the threads created by the main thread
 * have died - that is, when the global variable nLive is 0.
 * 
 * Note that although the threads in the example are created as joinable and are immediately
 * reaped on termination using pthread_join(), we don't need to use this approach in order
 * to find out thread termination status. We could have made the threads detached, removed
 * the use of pthread_join() and simply used the thread array (and associated global variables)
 * as the means of recording the termination of each thread.
 */
#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_cond_t threadDied=PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex=PTHREAD_MUTEX_INITIALIZER;
                /*Protects all of the following global variables*/
static int totThreads=0;                // Total number of threads created.
static int numLive=0;                   // Total # of thread alive or terminated but not joined.
static int numUnjoined=0;               // Number of terminated threads that have not been joined.
enum tstate {                           // Thread states.
    TS_ALIVE,                           // Thread is alive.
    TS_TERMINATED,                      // Thread terminated but not joined.
    TS_JOINED                           // Thread terminated and joined.
};
static struct {
    pthread_t tid;                      // ID of this thread.
    enum tstate state;                  // Thread state.
    int sleepTime;                      // Number of seconds to sleep.
} *thread;                              // Array of threads.

static void* threadFunc(                // Function executed by thread.
    void* arg)                          // Argument passed to the thread.
{
    int idx=*((int*) arg);              // Thread index.
    int status;                         // Status code.
    sleep(thread[idx].sleepTime);       // Simulate doing some work.
    printf("Thread %d terminating\n", idx);
    // Lock the mtx thread we will be doing somethings with the thread array.
    status=pthread_mutex_lock(&threadMutex);
    if (status!=0)
        errExitEN(status,"pthread_mutex_lock");
    numUnjoined++;                     // Number of terminated but unjoined threads ++
    thread[idx].state=TS_TERMINATED;   // Update the state.
    // Unlock the mutex.
    status=pthread_mutex_unlock(&threadMutex);
    if (status!=0)
        errExitEN(status,"pthread_mutex_unlock");
    // Signal the condition variable.
    status=pthread_cond_signal(&threadDied);
    if (status!=0)
        errExitEN(status,"pthread_cond_signal");
    return NULL;
}

int main(
    int argc,                           // Number of cmd-line arguments.
    char* argv[])                       // Array of cmd-line arguments.
{
    int status;                         // Status code.
    int idx;                            // Index into the thread array.
    if (argc < 2 || strcmp(argv[1], "--help")==0)
        usageErr("%s nsecs...\n", argv[0]);
    thread=calloc(argc - 1, sizeof(*thread)); // Create the thread array.
    if (thread==NULL)                   // Were we able to allocate the array?
        errExit("calloc");              // No, that's an error.
    // Create all of the threads.       //
    for (idx=0;idx<argc-1;idx++)        // For the number of input args.
    {                                   // Populate the thread object.
      thread[idx].sleepTime=getInt(argv[idx+1], GN_NONNEG, NULL); // Get the sleep time.
      status=pthread_create(&thread[idx].tid,NULL,threadFunc, (void*) idx);
      if (status!=0)                    // If we failed to create the thread.
        errExitEN(status,"pthread_create"); // Print error and exit.
      thread[idx].state=TS_ALIVE;       // Set the state to alive.
    }                                   // Done creating threads.
    totThreads=idx;                     // We created this many threads.
    numLive=totThreads;                 // All threads are alive.
    // Now wait (join) for (with) terminated threads to reap them.
    while (numLive>0)                   // While there are threads alive...
    {                                   // Lock the mutex.
      status=pthread_mutex_lock(&threadMutex);// Lock the mutex.
      if (status!=0)                    // Did we fail to lock the mtx?
        errExitEN(status,"pthread_mutex_lock");
      while (numUnjoined==0)            // While there are no orphaned threads...
      {                                 // Unlock the mutex, check state variables and wait.
        status=pthread_cond_wait(&threadDied, &threadMutex); // Wait for a signal.
        if (status!=0)                  // Did we fail to wait?
          errExitEN(status,"pthread_cond_wait");
      }                                 // Done waiting for a signal.
      for (idx=0;idx<totThreads;idx++)  // For each thread in the array...
      {                                 // Check their state
        if (thread[idx].state==TS_TERMINATED)// If the thread is terminated...
        {                               // We'll reapd it by joining it.
            status=pthread_join(thread[idx].tid,NULL);// Join the thread
            if (status!=0)              // If we failed to join the thread.
                errExitEN(status,"pthread_join"); // Print error and exit.
            thread[idx].state=TS_JOINED;// We have handled THIS thread.
            numLive--;                  // Remove THIS thread from the count.
            numUnjoined--;              // Remove THIS thread from the orphaned count.
            printf("Thread %d joined.\n", idx);// Print the thread ID.
        }                               // Done checking the thread array.
      }                                 // Done checking the thread array. 
      status=pthread_mutex_unlock(&threadMutex);// Unlock the mutex.
        if (status!=0)                  // Did we fail to unlock the mtx?
            errExitEN(status,"pthread_mutex_unlock");
    }                                   // Done waiting for threads to terminate.
    free(thread);                       // Free the thread array.
    thread=NULL;                        // Set the pointer to NULL.
    exit(EXIT_SUCCESS);                 // Exit the program.
}                                       // ------------- main -------------