#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "input.h"

static void discard_rest_of_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static int only_spaces(const char *s) {
    for (; *s; s++)
        if (!isspace((unsigned char)*s)) return 0;
    return 1;
}

int read_text(const char *prompt, char *dest, size_t size) {
    printf("%s", prompt);

    if (fgets(dest, (int)size, stdin) == NULL)
        return 0;                                   /* EOF */

    size_t len = strlen(dest);

    if (len > 0 && dest[len - 1] == '\n') {
        dest[len - 1] = '\0';                       /* whole line fit */
    } else if (len == size - 1) {
        /* Buffer filled without finding '\n': the line either has exactly
           size-1 characters or is longer. Peek at the next char. */
        int c = getchar();
        if (c != '\n' && c != EOF) {
            discard_rest_of_line();
            printf("Error: text longer than %zu characters.\n", size - 1);
            return 0;
        }
    }

    if (only_spaces(dest)) {
        printf("Error: empty field.\n");
        return 0;
    }
    return 1;
}

int read_int(const char *prompt, long min, long max, int *result) {
    char line[64];
    char *end;
    long value;

    printf("%s", prompt);
    if (fgets(line, sizeof line, stdin) == NULL)
        return 0;

    /* TODO 1: if the line has no '\n' and the buffer is full, discard the
               rest of the line and reject (same idea as read_text). */

    /* TODO 2: errno = 0; value = strtol(line, &end, 10); */

    /* TODO 3: reject if: nothing was converted (end == line),
               errno == ERANGE, or anything other than whitespace
               follows the number. */

    /* TODO 4: reject if value < min or value > max
               (and if it does not fit in an int, since result is int). */

    /* TODO 5: *result = (int)value; return 1; */

    (void)end; (void)value; (void)min; (void)max; (void)result;
    return 0;
}