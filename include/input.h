#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

// Return 1 on sucess, 0 on invalid input (message already printed)
int read_text(const char *prompt, char *dest, size_t size);
int read_int(const char *prompt, long min, long max, int *result);

#endif
