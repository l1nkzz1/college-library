#include <stdio.h>
#include <limits.h>
#include "book.h"
#include "input.h"

int main(void) {
    Book b;
    for (int i = 0; i < 6; i++) {
        if (read_int("ID: ", 1, INT_MAX, &b.id) &&
            read_text("Title: ", b.title, sizeof b.title) &&
            read_int("Year: ", YEAR_MIN, YEAR_MAX, &b.year))
            printf("OK: %d | %s | %d\n", b.id, b.title, b.year);
        else
            printf("-> rejected\n\n");
    }
    return 0;
}