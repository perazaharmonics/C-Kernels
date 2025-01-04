#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env;


static void doJump (
  int nvar,
  int rvar,
  int vvar)
{
  printf("Inside doJump(): nvar=%d rvar=%d vvar=%d.\n",nvar,rvar,vvar);
  longjmp(env,1);                  // Jump to point where setjmp was set
} 

int main (
  int argc,
  char* argv[])
{
  int nvar;                             // The # of variables.
  register int rvar;                    // A register int variables (allocate in reg if possible).
  volatile int vvar;                    // Volatile int variables. (See text)
  
  nvar=111;
  rvar=222;
  vvar=333;
  
  if (setjmp(env)==0)                   // Code executed after setjmp()
  {
    nvar=777;
    rvar=888;
    vvar=999;
    doJump(nvar,rvar,vvar);             // Code executed after longjmp()
  }
  else
    printf("After longjmp(): nvar=%d rvar=%d vvar=%d.\n",nvar,rvar,vvar);
  exit(EXIT_SUCCESS);
}
