#include <stdio.h>
#include "library.h"

static void check(const char *label, int got, int expected) {
  printf("%-42s got=%2d expected=%2d %s\n",
         label, got, expected, got == expected ? "OK" : "FAIL");
}

static Book mk(int id) {
  Book b;
  b.id = id;
  snprintf(b.title, sizeof b.title, "Title %d", id);
  snprintf(b.author, sizeof b.author, "Author %d", id);
  b.year = 2000;
  return b;
}

int main(void) {
  Library *lib = library_create();

  // registering a book creates its vertex
  check("add 10", library_add_book(lib, mk(10)), LIB_OK);
  check("add 20", library_add_book(lib, mk(20)), LIB_OK);
  check("add 30", library_add_book(lib, mk(30)), LIB_OK);
  check("vertex exists for 10", graph_has_vertex(lib->graph, 10), 1);
  check("consistent", library_is_consistent(lib), 1);
  check("history size", stack_size(lib->history), 3);

  // rejected registrations change nothing
  check("duplicate 10", library_add_book(lib, mk(10)), LIB_ERR_EXISTS);
  check("invalid id 0", library_add_book(lib, mk(0)), LIB_ERR_INVALID);
  check("bst size still 3", bst_size(lib->tree), 3);
  check("history still 3", stack_size(lib->history), 3);

  // edges
  check("add edge (10,20)", library_add_edge(lib, 10, 20), LIB_OK);
  check("duplicate edge (20,10)", library_add_edge(lib, 20, 10), LIB_OK);
  check("loop (10,10)", library_add_edge(lib, 10, 10), LIB_ERR_LOOP);
  check("edge with missing id", library_add_edge(lib, 10, 999), LIB_ERR_NOTFOUND);
  check("history after edge ops", stack_size(lib->history), 4);
  library_add_edge(lib, 10, 30);
  library_add_edge(lib, 20, 30);
  check("history = 6", stack_size(lib->history), 6);

  // removing a book removes its vertex and all incident edges
  check("remove book 10", library_remove_book(lib, 10), LIB_OK);
  check("book 10 gone from BST", library_find_book(lib, 10) == NULL, 1);
  check("vertex 10 gone", graph_has_vertex(lib->graph, 10), 0);
  check("edge (10, 20) gone", library_has_edge(lib, 10, 20), LIB_ERR_NOTFOUND);
  check("edge (20, 30) kept", library_has_edge(lib, 20, 30), 1);
  check("consistent after remove", library_is_consistent(lib), 1);
  check("remove nonexistent 999", library_remove_book(lib, 999), LIB_ERR_NOTFOUND);
  check("history = 7", stack_size(lib->history), 7);

  // removing edges
  check("remove edge (20, 30)", library_remove_edge(lib, 20, 30), LIB_OK);
  check("remove it again", library_remove_edge(lib, 20, 30), LIB_ERR_NOEDGE);
  check("history = 8", stack_size(lib->history), 8);

  // simulated failure: the rollback must leave everythings as before
  lib->fault = LIB_FAULT_VERTEX;
  check("vertex failure on add 40", library_add_book(lib, mk(40)), LIB_ERR_NOMEM);
  check("40 rolled back from BST", library_find_book(lib, 40) == NULL, 1);
  check("consistent after rollback", library_is_consistent(lib), 1);

  lib->fault = LIB_FAULT_HISTORY;
  check("history failure on add 41", library_add_book(lib, mk(41)), LIB_ERR_NOMEM);
  check("41 rolled back from BST", library_find_book(lib, 41) == NULL, 1);
  check("41 rolled back from graph", graph_has_vertex(lib->graph, 41), 0);
  check("consistent after rollback", library_is_consistent(lib), 1);

  lib->fault = LIB_FAULT_HISTORY;
  check("history failure on remove 20", library_remove_book(lib, 20), LIB_ERR_NOMEM);
  check("20 still in BST", library_find_book(lib, 20) != NULL, 1);
  check("20 still a vertex", graph_has_vertex(lib->graph, 20), 1);

  lib->fault = LIB_FAULT_HISTORY;
  check("history failure on add edge", library_add_edge(lib, 20, 30), LIB_ERR_NOMEM);
  check("edge rolled back", library_has_edge(lib, 20, 30), 0);

  check("add edge (20, 30) again", library_add_edge(lib, 20, 30), LIB_OK);
  lib->fault = LIB_FAULT_HISTORY;
  check("history failure on remove edge", library_remove_edge(lib, 20, 30), LIB_ERR_NOMEM);
  check("edge restored", library_has_edge(lib, 20, 30), 1);

  check("history only counts successes", stack_size(lib->history), 9);
  printf("\nLast actions:\n");
  stack_print_last(lib->history, 5);

  // bfs and path through the library
  check("path 20 -> 30", library_has_path(lib, 20, 30), 1);
  check("path with missing id", library_has_path(lib, 20, 999), LIB_ERR_NOTFOUND);
  library_free(lib);

  // capacity: the 101st registration is rejected without a partital record
  lib = library_create();
  int rc = LIB_OK;
  for (int id = 1; id <= 100; id++) rc = library_add_book(lib, mk(id));
  check("100th book", rc, LIB_OK);
  check("101st book rejected", library_add_book(lib, mk(101)), LIB_ERR_FULL);
  check("BST size stays 100", bst_size(lib->tree), 100);
  check("101 not in BST", library_find_book(lib, 101) == NULL, 1);
  check("consistent when full", library_is_consistent(lib), 1);
  check("history stays 100", stack_size(lib->history), 100);
  library_free(lib);

  return 0;
}
