#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
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
        return 0;                                   /* EOF */

    /* 1. Line too long: buffer full and no '\n' yet. */
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] != '\n' && len == sizeof line - 1) {
        int c = getchar();
        if (c != '\n' && c != EOF) {
            discard_rest_of_line();
            printf("Error: input too long.\n");
            return 0;
        }
    }

    /* 2. Convert. errno must be cleared first, strtol only sets it on error. */
    errno = 0;
    value = strtol(line, &end, 10);

    /* 3a. Nothing converted (empty line, "abc"). */
    if (end == line) {
        printf("Error: not a valid number.\n");
        return 0;
    }

    /* 3b. Overflow, or does not fit in an int. */
    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        printf("Error: number out of range.\n");
        return 0;
    }

    /* 3c. Trailing garbage ("12abc"). Only whitespace may follow. */
    for (; *end; end++) {
        if (!isspace((unsigned char)*end)) {
            printf("Error: not a valid number.\n");
            return 0;
        }
    }

    /* 4. Caller-defined interval. */
    if (value < min || value > max) {
        printf("Error: value must be between %ld and %ld.\n", min, max);
        return 0;
    }

    /* 5. Success. */
    *result = (int)value;
    return 1;
}