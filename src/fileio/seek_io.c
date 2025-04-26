#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main (                              // Process file offsets
  int argc,                             // Number of arguments
  char* argv[])                         // Argument list.
{                                       // ------------ main ---------------- //
  size_t len;                           // Byte count
  off_t offset;                         // Where to offset to inside the file.
  int fd_in, ap, j;                     // File descriptor, arg ptr, char counter.
  unsigned char* buf;                   // The input buffer.
  ssize_t nRead, nWrite=0;              // # of bytes read and written.
    // -------------------------------- //
    // If we have the correct amount of arguments and the user asks
    // for "--help", print use case.
    // -------------------------------- //
  if (argc <3 || strcmp(argv[1],"--help")==0)
    usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}...\n",
      argv[0]);
    // -------------------------------- //
    // File operations, open the input file for further processing
    // and apply file permissions.
    // -------------------------------- //
  fd_in=open(argv[1], O_RDWR | O_CREAT, // Open the input file
    S_IRUSR | S_IWUSR | S_IWGRP |
    S_IROTH | S_IWOTH);                 // rw-rw-rw- With these permissions.
  if (fd_in==-1)                        // Could we open the input file.
    errExit("open");                    // No, that's an error.
  for (ap=2; ap<argc; ap++)             // For the amount of input arguments..
  {
    switch (argv[ap][0])                // Get the first character, of the third argument.
    {                                   // And handle the case.
    // -------------------------------- //
    // Display bytes at the current offset in the input file.
    // -------------------------------- //
      case 'r':                         // Display bytes at current offset, as text.
      case 'R':                         // Display bytes at current offset, as Hex.
        len=getLong(&argv[ap][1],GN_ANY_BASE,argv[ap]);
        buf=malloc(len);                // Allocate memory for the input buffer.
        if (buf == NULL)                // Did we allocate any memory?
          errExit("malloc");            // No, that's an error.
        nRead=read(fd_in,buf,len);      // Read this much into the buffer.
        if (nRead==0)                   // Are we done reading?
          printf("%s: end-of-file.\n",argv[ap]);// Yes, so say that.
        else                            // Else, there's data to read.
        {                               // So, let's do that..
          printf("%s: ", argv[ap]);     // Print the argument
          for (j=0;j<nRead;j++)         // For the amounts of bytes read.
          {                             // .. process accordingly
            if (argv[ap][0]=='r')       // User want to print buf in txt?
              printf("%c",              // Yes, is it printable ?
                isprint(buf[j]) ? buf[j] : '?'); // Handle accordingly
            else                        // Else, user wants Hex representation.
              printf("%02x ",buf[j]);   // So do that.
          }                             // Done with for loop.
          printf("\n");                 // Add CRLF.
        }                               // Done with handling data in this form.
        free(buf);                      // Clear memory allocation.
        break;                          // Done with this case. Exit.
    // -------------------------------- //
    // Write bytes in the input file at the current offset.
    // -------------------------------- // 
      case 'w':                         // Write bytes at current offset.
        nWrite=write(fd_in,&argv[ap][1],strlen(&argv[ap][1]));
        if (nWrite==-1)                 // Did we write anything?
          errExit("write");             // No, that's an error.
        printf("%s: wrote %ld bytes\n", // Notify user how many bytes we wrote.
          argv[ap],(long) nWrite);      // From this command.
        break;                          // Done with this case.
    // -------------------------------- //
    // Change the file position pointer location
    // by this offset amount of bytes.
    // -------------------------------- //  
      case 's':                         // Change file offset ptr.
        offset=getLong(&argv[ap][1],GN_ANY_BASE,argv[ap]);// Convert to long.
        if (lseek(fd_in,offset,SEEK_SET)==-1)// Could we offset to this position?
          errExit("lseek");             // No, that's an error. Exit.
        printf("%s: seek succeded.\n",  // Yes. Notify user we succeded .. 
          argv[ap]);                    // .. using this command.
        break;                          // Done with this case.
      default:                          // We don't recognize that argument... 
        cmdLineErr("Argument must start with [rRws]: %s.\n",
          argv[ap]);                    // So we can't recover. 
    }                                   // Done with the switch
  }                                     // Done with argument list.
  exit(EXIT_SUCCESS);                   // Success if we got here.
}
