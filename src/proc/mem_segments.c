#include <stdio.h>
#include <stdlib.h>

char globBuf[65536];                    // Uninitialized data segment.
int primes[]={2,3,5,7};                 // Initialized data segment.

static int square (int x)               // Allocated in stack frame for square ()
{
  int result;                           // Allocated in stack frame for square ()
  result=x*x;
  return result;                        // Return value passed via register.
}
static void doCalc (int val)            // Allocated in stack frame for doCalc ()
{
  printf("The square of %d is %d.\n",val,square(val));
  if (val < 1000)
  {
    int t;                              // Allocated in stack frame for doCalc ()
    t = val*val*val;                    // The cube of val.
    printf("The cube of %d is %d.\n",val,t);
  }
}

int main (                              // Allocated in stack frame for main ()
  int argc,                             // The argument count.
  char* argv[])                         // The argument list.
{
  static int key=9973;                  // Initialized data segment.
  static char membuf[10240000];         // Uninitialized data segment.
  char* p;                              // Allocated in stack frame for main ().
  p=malloc(1024);                       // Points to memory in heap segment (free store).
  doCalc(key);                          // Calculate square and cube of the key.
  exit(EXIT_SUCCESS);                   // Exit the stack frame.
}
