#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "graph.h"
#include "library.h"
#include "input.h"
#include "sorting.h"

#define HISTORY_SHOWN 5

static void print_menu(void) {
  printf("LIBRARY DATA STRUCTURES SYSTEM\n");
  printf("1 - Register book\n");
  printf("2 - Search book\n");
  printf("3 - Remove book\n");
  printf("4 - List books (in-order)\n");
  printf("5 - Sort and show books by title\n");
  printf("6 - Add connection\n");
  printf("7 - Remove connection\n");
  printf("8 - Check direct connection\n");
  printf("9 - List reachable books\n");
  printf("10 - Check path between two IDs\n");
  printf("11 - Show history\n");
  printf("0 - exit\n");
}

static void print_book(const Book *b) {
  printf("ID: %d | Title %s | Author: %s | Year: %d\n",
         b->id, b->title, b->author, b->year);
}

static void print_book_visitor(const Book *b, void *ctx) {
  (void)ctx;
  print_book(b);
}

static void print_error(int rc) {
  switch (rc) {
    case LIB_ERR_INVALID:  printf("error: invalid argument.\n"); break;
    case LIB_ERR_EXISTS:   printf("error: that ID or connection already exists.\n"); break;
    case LIB_ERR_FULL:     printf("error: library is full (%d books).\n", GRAPH_MAX); break;
    case LIB_ERR_NOTFOUND: printf("error: ID not found.\n"); break;
    case LIB_ERR_LOOP:     printf("error: a book cannot be connected to itself.\n"); break;
    case LIB_ERR_NOEDGE:   printf("error: that connection doesn't exist.\n"); break;
    case LIB_ERR_NOMEM:    printf("error: out of memory!\n"); break;
    default:               printf("error: unknown error.\n"); break;
  } 
}

// each reader prints its ow error message so handlers just return on failure
static int read_id(const char *prompt, int *id) {
  return read_int(prompt, 1, INT_MAX, id);
}

static int read_two_ids(int *a, int *b) {
  return read_id("First ID: ", a) && read_id("Second ID: ", b);
}

// handlers

static void handle_add_book(Library *lib) {
  Book b;
  if (!read_id("ID: ", &b.id)) return;
  if (!read_text("Title: ", b.title, sizeof b.title)) return;
  if (!read_text("Author: ", b.author, sizeof b.author)) return;
  if (!read_int("Year (1-2026): ", YEAR_MIN, YEAR_MAX, &b.year)) return;

  int rc = library_add_book(lib, b);
  if (rc == LIB_OK) printf("Book %d registered.\n", b.id);
  else              print_error(rc);
}

static void handle_search(Library *lib) {
  int id;
  if (!read_id("ID: ", &id)) return;

  const Book *b = library_find_book(lib, id);
  if (b == NULL) printf("Book %d not found.\n",id);
  else           print_book(b);
}

static void handle_remove_book(Library *lib) {
  int id;
  if (!read_id("ID: ", &id)) return;

  int rc = library_remove_book(lib, id);
  if (rc == LIB_OK) printf("Book %d removed with its vertex and edges.\n", id);
  else              print_error(rc);
}

static void handle_list(Library *lib) {
  if (bst_size(lib->tree) == 0) {
    printf("No books registered.\n");
    return;
  }
  bst_inorder(lib->tree, print_book_visitor, NULL);
}

static void handle_sort(Library *lib) {
  Book *arr = NULL;
  int n = 0;

  if (!sorting_sorted_copy(lib->tree, &arr, &n)) {
    printf("Error: out of memory.\n");
    return;
  }
  if (n == 0) {
    printf("No books registered.\n");
    return;
  }
  for (int i = 0; i < n; i++) print_book(&arr[i]);
  free(arr);   // auxiliary array is released right after use
}

static void handle_add_edge(Library *lib) {
  int a, b;
  if (!read_two_ids(&a, &b)) return;

  int rc = library_add_edge(lib, a, b);
  if (rc == LIB_OK) printf("Connection (%d, %d) added.\n", a, b);
  else              print_error(rc);
}

static void handle_remove_edge(Library *lib) {
  int a, b;
  if (!read_two_ids(&a, &b)) return;

  int rc = library_remove_edge(lib, a, b);
  if (rc == LIB_OK) printf("Connection (%d, %d) removed.\n", a, b);
  else              print_error(rc);
}
static void handle_has_edge(Library *lib) {
  int a, b;
  if (!read_two_ids(&a, &b)) return;

  int rc = library_has_edge(lib, a, b);
  if (rc == 1)      printf("Books %d and %d are directly connected.\n", a, b);
  else if (rc == 0) printf("Books %d and %d are NOT directly connected.\n", a, b);
  else              print_error(rc);
}

static void handle_reachable(Library *lib) {
  int id;
  if (!read_id("Origin ID: ", &id)) return;

  List *reach = NULL;
  int rc = library_reachable(lib, id, &reach);
  if (rc != LIB_OK) {
    print_error(rc);
    return;
  }
  printf("Reachable from %d (BFS order): ", id);
  list_print(reach);
  list_free(reach);
}

static void handle_path(Library *lib) {
  int a, b;
  if (!read_two_ids(&a, &b)) return;

  int rc = library_has_path(lib, a, b);
  if (rc == 1)   printf("There is a path between %d and %d.\n", a, b);
  else if(rc == 0) printf("There is NO path between %d and %d.\n", a, b);
  else           print_error(rc);
}

static void handle_history(Library *lib) {
  stack_print_last(lib->history, HISTORY_SHOWN);
}

// main
int main(void) {
  Library *lib = library_create();
  if (lib == NULL) {
    printf("error: could not initialize the library.\n");
    return 1;
  }

  int choice = -1;
  do {
        print_menu();
        if (!read_int("Choose an option: ", 0, 11, &choice)) {
            if (feof(stdin)) break;   // input closed avoid looping forever
            choice = -1;
            continue;
        }

        switch (choice) {
            case 1:  handle_add_book(lib);    break;
            case 2:  handle_search(lib);      break;
            case 3:  handle_remove_book(lib); break;
            case 4:  handle_list(lib);        break;
            case 5:  handle_sort(lib);        break;
            case 6:  handle_add_edge(lib);    break;
            case 7:  handle_remove_edge(lib); break;
            case 8:  handle_has_edge(lib);    break;
            case 9:  handle_reachable(lib);   break;
            case 10: handle_path(lib);        break;
            case 11: handle_history(lib);     break;
            case 0:  break;
        }
    } while (choice != 0);

  library_free(lib);
  printf("bye.\n");
  return 0;
}
