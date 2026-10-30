#include <stdlib.h>
#include <string.h>
#include "sorting.h"

int compare_books(const Book *a, const Book *b) {
    int c = strcmp(a->title, b-> title);
    if (c != 0) return c;
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;
    return 0;
}

void insertion_sort_books(Book *arr, int n) {
   for (int i = 1; i < n; i++) {
    Book key = arr[i];
    int j = i - 1;
    while (j >= 0 && compare_books(&arr[j], &key) > 0) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

typedef struct {
  Book *arr;
  int   next;
} CopyCtx;

static void copy_visitor(const Book *book, void *ctx) {
  CopyCtx *c = ctx;
  c->arr[c->next++] = *book;   // copy by value
}

int sorting_sorted_copy(const BST *t, Book **out, int *count) {
  if (t == NULL || out == NULL || count == NULL) return 0;

  *out = NULL;
  *count = 0;

  int n = bst_size(t);
  if (n == 0) return 1;

  Book *arr = malloc((size_t)n * sizeof *arr);
  if (arr == NULL) return 0;

  CopyCtx ctx = { arr, 0 };
  bst_inorder(t, copy_visitor, &ctx);

  insertion_sort_books(arr, n);

  *out = arr;
  *count = n;
  return 1;
}

