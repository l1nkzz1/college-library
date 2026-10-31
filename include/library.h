#ifndef LIBRARY_H
#define LIBRARY_H

#include "book.h"
#include "bst.h"
#include "graph.h"
#include "list.h"
#include "stack.h"

#define LIB_OK             1
#define LIB_ERR_INVALID   -1
#define LIB_ERR_EXISTS    -2
#define LIB_ERR_FULL      -3
#define LIB_ERR_NOTFOUND  -4
#define LIB_ERR_LOOP      -5
#define LIB_ERR_NOEDGE    -6
#define LIB_ERR_NOMEM     -7

typedef enum {
  LIB_FAULT_NONE,
  LIB_FAULT_VERTEX,
  LIB_FAULT_HISTORY
} LibraryFault;

typedef struct {
  BST    *tree;
  Graph  *graph;
  Stack  *history;
  LibraryFault fault;
} Library;

Library *library_create(void);
void     library_free(Library *lib);

// books BST + vertex, always together
int         library_add_book(Library *lib, Book book);
int         library_remove_book(Library *lib, int id);
const Book *library_find_book(const Library *lib, int id);

// connections edges
int library_add_edge(Library *lib, int a, int b);
int library_remove_edge(Library *lib, int a, int b);
int library_has_edge(const Library *lib, int a, int b);

// queries
int library_reachable(const Library *lib, int origin, List **out);
int library_has_path(const Library *lib, int a, int b);

// if every book has a vertex and the counts match
int library_is_consistent(const Library *lib);

#endif
