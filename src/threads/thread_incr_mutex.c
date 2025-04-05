/**
 * This program provides a simple example of issues that can arise when two threads
 * attempt to access a shard resource non-atomically. This program creates two threads
 *which starting function implements a loop that continusly increments a global variable,
 * glob by copying glob into the local variable loc, incrementing loc and then copying
 * the result back to glob. (Since loca is an automatic variable allocated on the
 * per thread stack, each thread has its own copy of this variable. This program
 * is a modified version of bad_thread_access_incr.c that uses a mutex to protect
 * access to the global variable and achieve thread synchronization.
 */

 #include <pthread.h>
 #include "tlpi_hdr.h"

 static volatile int glob=0;            // Volatile tells the cimpiler not to optimize it.
 static pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER; // Mutex for glob.

 static void* threadFunc(               // Function executed per thread.     
    void* arg)                          // Argument passed to the thread.
{
    int loops=*((int*) arg);            // Cast to an int and dereference.
    int loc;                            // Local copy of global variable.
    int status;                         // Status code.
    for (int j=0; j<loops; j++)         // Loop to increment the global var.
    {
        status=pthread_mutex_lock(&mtx);// Lock the mutex.
        if (status!=0)                  // Did we fail to lock the mutex?
            errExitEN(status,"pthread_mutex_lock");
        loc=glob;                       // Copy the global variable.
        loc++;                          // Increment local copy.
        glob=loc;                       // Update the global variable.
        status=pthread_mutex_unlock(&mtx); // Unlock the mutex.
        if (status!=0)                  // Did we fail to unlock the mutex?
            errExitEN(status,"pthread_mutex_unlock");
    }
    return NULL;                        // Return value is not used.
}

int main(
    int argc,                           // The argument count.
    char* argv[])                       // The argument vector.
{
    pthread_t t1, t2;                   // The thread handles.
    int loops;                          // Number of loops to run inside thread.
    int status;                         // Status code returned by pthread_create().
    loops=(argc>1)?getInt(argv[1],GN_GT_0,"num-loops"):10000000;
    status=pthread_create(&t1,NULL,threadFunc,&loops);
    if (status!=0)                      // Did pthread_create() fail?
        errExitEN(status,"pthread_create");
    status=pthread_create(&t2,NULL,threadFunc,&loops);
    if (status!=0)                      // Did pthread_create() fail?
        errExitEN(status,"pthread_create");
    if (t1!=pthread_self())             // Ensure that this is not the main thread.
    {
        status=pthread_join(t1,NULL);   // Wait for the first thread to terminate.
        if (status!=0)                  // Did pthread_join() fail?
           errExitEN(status,"pthread_join");
    }
    if (t2!=pthread_self())             // Ensure that this is not the main thread.
    {
        status=pthread_join(t2,NULL);   // Wait for the second thread to terminate.
        if (status!=0)                  // Did pthread_join() fail?
            errExitEN(status,"pthread_join");
    }
    printf("glob=%d\n",glob);           // Print the value of the global variable.
    exit(EXIT_SUCCESS);                 // Exit and indicate success.
}