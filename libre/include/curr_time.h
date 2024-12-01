#include <locale.h>
#include <sys/time.h>

// Returns a pointer to statically allocated string, or NULL on error.
char* currTime(const char* fmt);
