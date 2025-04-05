/** Demonstrates various time manipulation methods related to software clock.
* The DisplayProcessTime() functions prints the message supplied by the caller, 
* and then uses clock() and times() to retrieve and display the process process
* times. The main program makes an actual call to DisplayProcessTime() and then''
* executes a loop that consumes some CPU time by repeatedly calling getppid(),
* before again calling DisplayProcessTime() to see how much CPU time has been
* consumed.
*/

#include <sys/times.h>
#include <time.h>
#include "tlpi_hdr.h"

static void DisplayProcessTime(         // Display message and process times.
  const char* msg)                      // When did we call this function ?
{
  struct tms now;                       // The process time structure
  clock_t cTime;                        // The clock time
  static long cTicks=0;                 // The clock ticks.
  
  if (msg != NULL)                      // Anything to say?
    printf("%s", msg);                  // Yes, print it.
  if (cTicks == 0)                      // Any 'ticks' counted ?
  {                                     // No, fetch clock ticks on first call.
     cTicks=sysconf(_SC_CLK_TCK);       // get CLK_TCKS per second.
     if (cTicks == -1)                  // Could we get ticks/second.
       errExit("sysconf");              // No, that's an error.
  }                                     // Done fetching ticks on first call.
  cTime=clock();                        // Get CPU time used by curr process.
  if (cTime == -1)                      // Could we get consumed CPU time?
    errExit("clock");                   // No, that's an error.
  printf("      clock() returns: %ld clocks-per-sec (%.2f secs).\n",
    (long) cTime, (double) cTime/CLOCKS_PER_SEC);
  
  if (times(&now) == -1)                // Could we populate Proc. time struct?
    errExit("times");                   // No, that's an error.
  printf("      times() yields: user CPU=%.2f; system CPU: %.2f .\n",
    (double) now.tms_utime / cTicks,
    (double) now.tms_stime / cTicks);
}
// Waste CPU time for scientific purposes.
int main (                              // Obtains a process CPU time.
  int argc,                             // The number of arguments.
  char* argv[])                         // The argument list.
{                                       // -------------- Main --------------- //
  int nCalls;                           // # of times we called DisplayProcessTime()
  int j;                                // Loop iterator.
  printf("CLOCKS_PER_SEC=%ld  syscond(_SC_CLK_TCK)=%ld.\n\n",
    (long) CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));
  DisplayProcessTime("At program start.\n");// Get process time
                                        //
  // ---------------------------------- //
  // Get the input argument to determine our iterations.
  // This will determine how much CPU time we consume
  // ---------------------------------- //
                                        //
  nCalls=(argc >1)?getInt(argv[1],GN_GT_0,"num-calls"):10000000;
  for (j=0; j<nCalls; j++)              // For nCalls
    (void) getppid();                   // Get the ppid().
  DisplayProcessTime("After getppid() loop.\n");// Get the process time.
  exit(EXIT_SUCCESS);                    // Success if we got here
}                                       // -------------- Main --------------- //
