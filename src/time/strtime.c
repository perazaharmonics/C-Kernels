#define _XOPEN_SOURCE
#include <time.h>
#include <locale.h>
#include "tlpi_hdr.h"

/** Takes a cmd line argument containing a date and time, converts this to
* a broken-down time using strptime(), and then displays the result performing
* the reverse conversion using strftime(). The program takes three argument
* of which the first two are required. The first argument is the string 
* containing a date and time. The second argument is the format specification
* to be used by strptime() to parse the first argument.
*/

#define SBUF_SIZE 1000

int main (
  int argc,                             // The argument count.
  char* argv[])                         // The provided argument list.
{
  struct tm tm;                         // The broken down time.
  char sbuf[SBUF_SIZE];                 // Where to encode the date-time.
  char* ofmt;                           // The output format.
  // ---------------------------------- //
  // Verify the input arguemnt count, Do they want help?
  // ---------------------------------- //
  if (argc < 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s input-date-time in-format [out-format].\n", argv[0]);
    
  // ---------------------------------- //
  // Set local settings as the conversion standard.
  // ---------------------------------- //
  if (setlocale(LC_ALL, "") == NULL)    // Use local settings in conversion
    errExit("setlocale");
  memset(&tm, 0, sizeof(struct tm));    // Initialize 'tm' struct.
  
  // ---------------------------------- //
  // Process the input character until can't anymore
  // ---------------------------------- //
  if (strptime(argv[1], argv[2], &tm) == NULL)// Packed broken-down time struct ?
    fatal("strptime");                  // No, that's an error.
  // tm_isdst is not set by strptime(); 
  // tells mktime() to determine if DST is in effect.  
  tm.tm_isdst == -1;
  printf("Calendar time (seconds since Epoch): %ld.\n", (long) mktime(&tm));
  // ---------------------------------- //
  // Get the ouput encoding format if provided, else use default.
  // ---------------------------------- //
  ofmt = (argc > 3) ? argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
  if (strftime(sbuf, SBUF_SIZE, ofmt, &tm) == 0)
    fatal("strftime returned 0");
  printf("strftime() yields: %s.\n", sbuf);
  exit(EXIT_SUCCESS);                   // Success.             
}
