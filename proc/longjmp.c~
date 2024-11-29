#include <setjmp.h>
#include "tlpi_hdr.h"

static jmp_buf env;

static void f2 (void)
{
  longjmp(env, 2);                      // Do a longjmp, return 2 to indicate who called you.
}

static void f1 (int argc)
{
  if (argc==1)
    longjmp(env,1);                     // Do a longmp, return 1 to indicate who called you.
  f2();
}

int main (int argc, char* argv[])
{
  switch(setjmp(env))                   // Save environment at this point to return to.
  {
    case 0:                             // Return after initial setjmp
      printf("Calling f1() after initial setjmp().\n");
      f1(argc);                         // Never returns ...
      break;                            // But this is good form.
      
    case 1:
      printf("We jumped back from f1().\n");
      break;
      
    case 2:
      printf("We jumped back from f2().\n");
      break;
  }

}
