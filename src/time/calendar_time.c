// =================================== calendar_time.c ======================= //
// This program retrieves the current calendar time and then uses various
// time-conversion functions and displays their results/
//
// Author:
// J. EP, J. Enrique Peraza
//
// Reference:
//  The Linux Programming Interface, Michael Kerrisk, Chapter 10
//
// =========================================================================== //


#include <locale.h>
#include <time.h>
#include <sys/time.h>
#include "tlpi_hdr.h"

#define SECONDS_IN_TROPICAL_YEAR (365.24219 * 24 * 60 * 60)

int main (
  int argc,                             // The argument count
  char* argv[])                         // The argument list
{
  time_t now;                           // Seconds since Unix Epoch fmt.
  struct tm* gmp;                       // Pointer to GMT broken down time.
  struct tm* locp;                      // Pointer to local broken down time.
  struct tm gm;                         // GMT broken down time structure.
  struct tm loc;                        // Local broken down time structure.
  struct timeval tv;                    // Tval struct sec and usec precision
  
  now=time(NULL);                       // Get number this # of secs since Epoch.
  printf("Seconds since Unix Epoch: %ld.", (long) now);
  printf(" (about %6.3f years)\n", now/SECONDS_IN_TROPICAL_YEAR);
  
  if (gettimeofday(&tv, NULL) == -1)    // Could we get calendar time ?
    errExit("gettimeofday");            // No that's an error.
  printf("  gettimeofday() returned %ld secs, %ld microsecs.\n",
    (long) tv.tv_sec, (long) tv.tv_usec);
    
  gmp= gmtime(&now);                    // Get GMT broken-down time.
  if (gmp == NULL)                      // Could we get GMT broken-down time?
    errExit("gmtime");                  // No, that's an error.
  gm = *gmp;                            // Save a copy of GMT broken down time.
  printf("Broken down by gmtime():\n");
  printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d ", gm.tm_year,
      gm.tm_mon, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec);
  printf("wday=%d yday=%d isdst=%d.\n",gm.tm_wday, gm.tm_yday, gm.tm_isdst);
  
  locp=localtime(&now);                 // Get local broken-down time.
  if (locp == NULL)                     // Could we get GMT broken-down time?
    errExit("localtime");               // No, that's an error.
  loc=*locp;                            // Get a copy of the local time struct.
  printf("Broken down time by localtime():\n");
  printf("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d ",
      loc.tm_year, loc.tm_mon, loc.tm_mday, loc.tm_hour, loc.tm_min, loc.tm_sec);
  printf("wday=%d yday=%d isdst=%d.\n", loc.tm_wday, loc.tm_yday, loc.tm_isdst);
  printf("asctime() formats the gmtime() value as:  %s.\n", asctime(&gm));
  printf("ctime() formats the time() value as: %s.\n",ctime(&now));
  printf("mktime() of gmtime() value:     %ld secs.\n",(long) mktime(&gm));
  printf("mktime() of localtime() value:  %ld secs.\n",(long) mktime(&loc));
  
  exit(EXIT_SUCCESS);   
}
