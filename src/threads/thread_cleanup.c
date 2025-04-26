/**
 * Example use of cleanup handlers in threads. The main program creates a thread (8)
 * whose first action are to allocate a block of memory (3). whose location is stored
 * in buf, and then lock the mutex mtx (4). Since the thread may be cancelled it uses
 * pthread_cleanup_push() (5) to install a cleanup handler that is called with the address
 * stored in buf. If it is invoked, the cleanup handler deallocates the freed memory (1)
 * and unlocks the mutex (2). The thread then enters a loop waiting for the condition variable
 * cond to be signaled (6). This loop will terminate in one of two ways, depending on whether
 * the program is supplied with a cmd line arg:
 * If no cmd line arg is supplied the thread is canceled by main() (9). In this case,
 * cancellation will occur at the call o=to pthread_cond_wait() (6) which is one of the
 * cancellation points defined by the standards. As part of cancellation, the cleanup handler
 * established using pthread_cleanup_push() is invoked automatically. (When a pthread_cond_wait()
 * call is canceled, the mutex is automatically relocked before the cleanup handler is invoked.
 * This means the mutex can be be safely unlocked in the cleanup handler.)
 * 
 * If a cmd line arg is supplied, the condition variable is signaled (10) after the associated
 * global variable, glob, is first set to a nonzero value. In this case the thread falls through to
 * execute pthread_cleanup_pop() (7), which given a nonzero argument, also causes the cleanup
 * handler to be invoked.
 * 
 * The main program joins with the terminated thread (11), and reports whether the thread was 
 * caneled or terminated normally.
 */
#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
static int glob=0;                      // Predicate variable.

static void CleanupHandler (void* arg)
{                                       // --------- CleanupHandler ----------
  int s=0;                              // Status code.
  printf("Cleanup: freeing block at %p.\n",arg);
  free(arg);                            // (1) free block pointed by arg.
  s=pthread_mutex_unlock(&mtx);         // (2) Unlock mtx, we freed the threads data block.
  if (s!=0)                             // Error unlocking mutex?
    errExitEN(s,"pthread_mutex_unlock");// Yeah, exit with error.
}                                       // --------- CleanupHandler ----------

static void* ThreadFunc (void* arg)
{                                       // ----------- ThreadFunc -----------
  int s=0;                              // Status code.
  void* buf=NULL;                       // Buffer allocated by thread.
  buf=malloc(0x10000);                  // (3). Not a cancellation point.
  printf("Thread: allocated memory at: %p.\n",buf);
  s=pthread_mutex_lock(&mtx);           // (4) not a cancellation point.
  if (s!=0)                             // Falied to lock this thread's mutex?
    errExitEN(s,"pthread_mutex_lock");  // Yes, exit with error.
  pthread_cleanup_push(CleanupHandler,buf);// (5) push cleaner to the stack.
  while (glob==0)                       // While predicate global var is 0
  {                                     // ...
    s=pthread_cond_wait(&cond,&mtx);    // (6) a cancellation point.
    if (s!=0)                           // Did we get an error?
      errExitEN(s,"pthread_cond_wait"); // Yeah, exit with error.
  }                                     // Done waiting with while loop.
  printf("Thread: condition wait loop complete.\n");
  pthread_cleanup_pop(1);               // (7) Execute cleanup handler.
  return NULL;                          // We are done.
}                                       // ----------- ThreadFunc -----------

int main (                              // Our main function.
    int argc,                           // The argument count.
    char* argv[])                       // The argument list.
{                                       // ----------- main ----------------
  pthread_t t;                          // Our thread ID.
  void* res;                            // Result of pthread_join.
  int s;                                // Our status code.
  s=pthread_create(&t,NULL,ThreadFunc,NULL);// (8) Create the thread.
  if (s!=0)                             // Did we get an error crating thread?
    errExitEN(s,"pthread_create");      // Yeah, we gotta error out.
  sleep(2);                             // Give thread a chance to get started.
  if (argc==1)                          // Did we provide any input args?
  {                                     // No.
    printf("main:    about to cancel thread.\n");
    s=pthread_cancel(t);                // (9) Cancel the thread.
    if (s!=0)                           // Erro cancelling thread?
      errExitEN(s,"pthread_cancel");    // Yeah, error out.
  }                                     // Done with no argument passed.
  else                                  // Else the user provided an argument.
  {                                     // So let's call cleanup functions.
    printf("main:    about to signal condition variable.");
    glob=1;                             // Set predicate var.
    s=pthread_cond_signal(&cond);       // (10) signal condition variable.
    if (s!=0)                           // Did we fail to signal condition var?
      errExitEN(s,"pthread_cond_signal");// Yeah, exit with error.
  }                                     // Done signaling with condition variable.
  s=pthread_join(t,&res);               // (11) Join the thread to wait for it to finish.
  if (s!=0)                             // Failed to join the thread?
    errExitEN(s,"pthread_join");        // Yeah, exit with error code.
  if (res==PTHREAD_CANCELED)            // Was the thread canceled?
   printf("main:     thread was canceled.\n");
  else                                  // Else, thread exited normally.
   printf("main:      thread exited succesfully.\n");
  exit(EXIT_SUCCESS);                   // If we got here we were successful. 
}                                       // ----------- main ----------------