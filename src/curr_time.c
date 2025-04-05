#include <time.h>
#include "curr_time.h"                  // Declares function defined here.

#define BUF_SIZE 1000

/** Return a string containing the current time formatted according to the 
* specification in 'fmt' (see strftime(3) for specifiers).
* If 'format' is NULL, we use "%c" as a specifier (which gives the date and
* time as for ctime(3), but without the the trailing newline).
* Returns NULL on error.
*/
// Nonreentrant.
char* currTime (                        // Encode a string using this format
  const char* fmt)                      // The format to encode.
{                                       // ----------- currTime ------------- //
  static char buf[BUF_SIZE];            
  time_t now;                           // The elapsed seconds from Unix Epoch.
  size_t siz;                           // The size of string to encode.
  struct tm* tm;                        // The broken-down time structure.
  
  now=time(NULL);                       // How many seconds since Unix Epoch now?
  tm = localtime(&now);                 // Populate now in broken time struct.
  if (tm == NULL)                       // Did we put anything in struct?
    return NULL;                        // No, that's an error.
  // Get the bytes we encoded into buf.
  siz=strftime(buf, BUF_SIZE, (fmt != NULL) ? fmt: "%c", tm);
  // Did we encode anything ? Return NULL if we did not. 
  // Otherwise, return the encoded string.
  return (siz == 0) ? NULL : buf; 
}
