#ifndef SORTING_H
#define SORTING_H

#include "book.h"
#include "bst.h"

// Compares by title (strcmp) ties broken by ascending id
int compare_books(const Book *a, const Book *b);

// inplace insertion sort over an array of book values
void insertion_sort_books(Book *arr, int n);


int sorting_sorted_copy(const BST *t, Book **out, int *count);

#endif
