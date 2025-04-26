/**
 * This program shows a reimplementation of strerror() that uses thread-specific data
 * (TSD) t0 ensure thread safety. The first step performed by the revised strerror() is
 * to call pthread_once() (4) ro ensure that invocation of this function (from any thread)
 * calls createKey() (2). The createKey() function calls pthread_key_create() to allocate
 * a thread-specific data key that is stored in the global variable strerrorKey (3). The 
 * call to pthread_create_key() also records the destructor function (1) that will be used
 * to free thread specific buffers corresponding to the key. The strerror() function then
 * calls pthread_getspecific() (5) to retrieve the address of this thread's unique buffer
 * corresponding to to strerrorKey. If pthread_getspecific() returns NULL, then
 * this thread is calling strerror() for the first time, and so the functions
 * allocates a new buffer using malloc() (6) and saves the address of the buffer using
 * pthread_setspecific() (7). If pthread_getspecific() returns a non-NULL value, then 
 * that pointer refers to an existing buffer that was allocated when this thread 
 * previously called strerror(). 
 */
#define _GNU_SOURCE                     // Get _sys_nerr and _sys_errlist from stdio.h
#include <stdio.h>                      
#include <string.h>                     // Get declaration of strerror()
#include <pthread.h>
#include "tlpi_hdr.h"

static pthread_once_t once=PTHREAD_ONCE_INIT; // Used to ensure that createKey() is called only once
static pthread_key_t key;                // Thread-specific data key
#define BUF_SIZ 256                      // Size of buffer for error message

// (1) Destructor function to free thread-specific buffer
static void Destructor (
    void* buf)                          // Pointer to buffer to free
{                                       // ------------- Destructor -------------
    free(buf);                          // Free the buffer
}                                       // ------------- Destructor -------------
// (2) Function to create thread-specific data key
static void CreateKey (void)
{                                       // ------------- CreateKey -------------
    int s;                              // Status code
    // Allocate a unique thread-specific data key and save the address of the
    // destructor function for thread-specific data buffers. (3)
    s=pthread_key_create(&key,Destructor); // Create the key
    if (s!=0)                           // Did we fail to create the key?
        errExitEN(s,"pthread_key_create"); // Yes, print error and exit.
}                                       // ------------- CreateKey -------------
char* Strerror (int errno)
{                                       // ------------- Strerror --------------       
  int s;                                // Status code.
  char* buf=NULL;                       // Our thread specific data.
  // (4) Make first caller allocate key for thread-specific data.
  s=pthread_once(&once,CreateKey);      // create a key for thread specific data block.
  if (s!=0)                             // Fail to provide keys to data block?
    errExitEN(s,"pthread_once");        // Yes, so exit with error.
  // (5) Retrieve address of this threads unique buffer.
  buf=pthread_getspecific(key);         // Get the key for this TSD data block.
  if (buf==NULL)                        // Have we been to this address before?
  {                                     // No, that's why it is NULL.
    buf=malloc(BUF_SIZ);                // Allocate this many bytes. (6)
    if (buf==NULL)                      // Could we separate this many bytes?
      errExit("malloc");                // No, exit with error.
    // (7) save the specific address of this TSD buffer.
    s=pthread_setspecific(key,buf);     // We have alocated in this address with this key.
    if (s!=0)                           // Error saving the information for TSD buffer.
      errExitEN(s,"pthread_setspecific()");
  }                                     // Done saving TSD.
  if (errno<0||errno>=_sys_nerr||_sys_errlist[errno]==NULL)
    snprintf(buf,BUF_SIZ,"Unknown error %d",errno);
  else
  {
    snprintf(buf,BUF_SIZ,"%s",_sys_errlist[errno]);// Copy bufsiz-1 bytes.
    buf[BUF_SIZ-1]='\0';                // Ensure null termination.
  }                                     // Done copying error code.
  return buf;                           // Return the error string.
}