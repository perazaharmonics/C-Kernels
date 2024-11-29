// =========================================t_readv.c===================================================== //
// ReacV() System Call: Performs scatter input, where a contiguous sequence of bytes from the file referred
// to by file descriptor fd and places ("scatters") these bytes into the buffers specified by the struct iov.
// Each of these buffers, starting with the buffer placed at iov[0], is completely filled before readv()
// proceeds with the next buffer. This process is done atomically.
// ======================================================================================================= //

#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main (                              // --------------------------------------------------------------- //
  int argc,                             // The number of arguments in the argument list.
  char* argv[])                         // The argument list in itself.
{                                       // --------------------------------------------------------------- //
  int fd_in;                            // The file descriptor number.
  struct iovec iov[3];                  // The set of buffers to transfer data from/to.
  const int iovcnt=3;                   // The amount of iov buffers we expect.                  
  struct stat myStruct;                 // First buffer.
  int x;                                // Second buffer.
 #define STR_SIZE 100                   // Length of third buffer.
  char str[STR_SIZE];                   // Third buffer.
  ssize_t nRead, mNeed=0;               // Bytes read, and total required bytes to fille buffer.
  if (argc != 2 || strcmp(argv[1],"--help"))// Bad # of args or usr needs help?                          
    usageErr("%s file.\n",argv[0]);     // Yes, notify usr.
  fd_in=open(argv[1],O_RDONLY);         // Get the open file descriptor (fd=0).
  if (fd_in==-1)                           // Did we open a file?
    errExit("open");                    // No, that's an error ):
    // -------------------------------- //
    // Assign the base addresses and length of buffers.
    // -------------------------------- //
  iov[0].iov_base=&myStruct;            // Base address of the first iov buffer.
  iov[0].iov_len=sizeof(struct stat);   // The expected width of that buffer.
                                        //
  iov[1].iov_base=&x;                   // Base address of the second iov buffer.
  iov[1].iov_len=sizeof(x);             // The expected width of that buffer.
                                        //
  iov[2].iov_base=str;                  // The base address of the third buffer.
  iov[2].iov_len=STR_SIZE;              // The expected width of that buffer.
  mNeed+=iov[0].iov_len+iov[1].iov_len+iov[2].iov_len; // We need this much in total.
  nRead=readv(fd_in,iov,iovcnt);        // Scatter input read the input file.
  if (nRead==-1)                        // Did we read anything?
    errExit("readv");                   // No, that's an unrecoverable error.
  if (nRead < mNeed)                    // Did we read enough to fill the buffers.
    printf("Read fewer bytes than requested.\n");// No, just notify usr.
  printf("total bytes requested: %ld; bytes read: %ld.\n",
    (long) mNeed, (long) nRead);        
  exit(EXIT_SUCCESS);                   // If we got here, we were successful.
}
