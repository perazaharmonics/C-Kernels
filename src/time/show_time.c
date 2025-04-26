#include <time.h>
#include <locale.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 200

int main(
  int argc,
  char* argv[])
{
  time_t now;                           // Time elapsed since Unix Epoch.
  struct tm* loc;                       // Broken down time struct.
  char buf[BUF_SIZE];                   // Where to encode output.
  
  if (setlocale(LC_ALL, "") == NULL)    // Use local settings as conversion std.
    errExit("setlocale");               // Error out if that fails.
  now=time(NULL);                       // Get now in seconds since Epoch.
  printf("ctime() of time() value is: %s.\n", ctime(&now));
  
  loc=localtime(&now);                  // Convert that to broken-down time.
  if (loc == NULL)                      // Could we convert to broken-down ?
    errExit("localtime");               // Error out if not.
  printf("asctime() of local time is: %s.\n",asctime(loc));
  // ---------------------------------- //
  // Try to print broken down time with TZ information.
  // ---------------------------------- //
  if (strftime(buf,BUF_SIZE, "%A, %d %B %Y, %H:%M:%S %Z",loc) == 0)
    fatal("strftime returned 0");
  printf("strftime() of local time is: %s.\n",buf);
  
  exit(EXIT_SUCCESS);  
}
