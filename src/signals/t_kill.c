/**
 * Decscription: A demo of the use of kill(). These program takes two commands
 * line arguments, a process ID and a signal number and uses kill to send a signal
 * to the specified process. If signal 0 (the null signal) is specified, then the
 * program reports on the existence of the target program. 
 */
 
 #include <signal.h>
 #include "tlpi_hdr.h"
 
 int main(                              // -------- kill procs --------------- //
   int argc,                            // The number of arguments.
   char* argv[])                        // The variable argument list.
{                                       // ------------- main ---------------- //
  int status;                           // The kill return status.
  int sig;                              // The signal to send.
  // ---------------------------------- //
  // User asking for help?              
  // ---------------------------------- //
  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s pid sig-num\n", argv[0]);// Yeah, help them.
  // ---------------------------------- //
  // Where the main action happens.
  // ---------------------------------- //
  sig=getInt(argv[2], 0, "sig-num");    // Get the signal for usr input.
  status=kill(getLong(argv[1], 0, "pid"), sig);// Kill this proccess.
                                        //
  if (sig != 0)                         // Did the user send a null signal?
  {                                     // No, it must be something else.
    if (status == -1)                   // Did we get an error?
      errExit("kill");                  // Yes, so exit.
  }                                     // Done checking for errored null sig.
  else                                  // Else, we sent it and maybe succeded.
  {                                     // Let's verify.
    if (status == 0)                    // Did the kill return zero?
      printf("Process exists and we can send it a signal.\n");
    else                                // Else, kill is nonzero on return.
    {                                   // So check errno status.
       if (errno == EPERM)              // Did we get access permission err?
         printf("Process exists, but we don't have "
           "permission to send it a signal.\n");// Yes.
       else if (errno == ESRCH)         // Did we find the process?
         printf("Process does not exist!.\n");// No, it does not exist.
       else                             // Else, something bad happened.
         errExit("kill");               // So exit, we can't recover.
    }                                   // Done with non-zero return.        
  }                                     // Done handling null signal.
  exit(EXIT_SUCCESS);                   // If we got here we succeded.
}                                       // ------------- main ---------------- //
