#include <stdlib.h>
#include "library.h"
#include "bst.h"
#include "graph.h"
#include "stack.h"

// helpers

// translates a graph return code into a library return code.
// non-negative values (1 or 0 from has_edge/has_path) passthrough.
static int from_graph(int rc) {
  if (rc >= 0) return rc;
  switch (rc) {
    case GRAPH_ERR_INVALID:  return LIB_ERR_INVALID;
    case GRAPH_ERR_FULL:     return LIB_ERR_FULL;
    case GRAPH_ERR_EXISTS:   return LIB_ERR_EXISTS;
    case GRAPH_ERR_NOTFOUND: return LIB_ERR_NOTFOUND;
    case GRAPH_ERR_LOOP:     return LIB_ERR_LOOP;
    case GRAPH_ERR_NOEDGE:   return LIB_ERR_NOEDGE;
    case GRAPH_ERR_NOMEM:    return LIB_ERR_NOMEM;
  }
  return LIB_ERR_INVALID;
}

// returns 1 and consumes the fault, if the given fault was requested
static int fault_hits(Library *lib, LibraryFault f) {
  if (lib->fault == f) {
    lib->fault = LIB_FAULT_NONE;
    return 1;
  }
  return 0;
}

// pushes an action to the history. returns 1 on sucess, 0 on failure
static int record(Library *lib, ActionType type, int id1, int id2) {
  if (fault_hits(lib, LIB_FAULT_HISTORY)) return 0;
  Action a = { type, id1, id2 };
  return stack_push(lib->history, a);
}

// create and free

Library *library_create(void) {
  Library *lib = calloc(1, sizeof *lib);
  if(lib == NULL) return NULL;

  lib->tree    = bst_create();
  lib->graph   = graph_create();
  lib->history = stack_create();
  lib->fault   = LIB_FAULT_NONE;

  if (lib->tree == NULL || lib->graph == NULL || lib->history == NULL) {
     library_free(lib);
     return NULL;
  }
  return lib;
}

void library_free(Library *lib) {
  if (lib == NULL) return;
  bst_free(lib->tree);          // all of these accept NULL
  graph_free(lib->graph);
  stack_free(lib->history);
  free(lib);
}

// books

int library_add_book(Library *lib, Book book) {
  if (lib == NULL) return LIB_ERR_INVALID;

  // insert into the BST
  int rc = bst_insert(lib->tree, book);
  if (rc == BST_DUPLICATE) return LIB_ERR_EXISTS;
  if (rc == BST_INVALID)   return LIB_ERR_INVALID;
  if (rc == BST_NOMEM)     return LIB_ERR_NOMEM;

  // create the vertex
  if (fault_hits(lib, LIB_FAULT_VERTEX))
    rc = GRAPH_ERR_NOMEM;
  else
    rc = graph_add_vertex(lib->graph, book.id);

  // if it fails undo the bst insertion
  if (rc != GRAPH_OK) {
    bst_remove(lib->tree, book.id);
    return from_graph(rc);
  }

  // record in the history
  if (!record(lib, ACTION_ADD_BOOK, book.id, 0)) {
    graph_remove_vertex(lib->graph, book.id);
    bst_remove(lib->tree, book.id);
    return LIB_ERR_NOMEM;
  }
  return LIB_OK;
}

int library_remove_book(Library *lib, int id) {
  if (lib == NULL) return LIB_ERR_INVALID;

  // the book must exist
  if (bst_search(lib->tree, id) == NULL) return LIB_ERR_NOTFOUND;

  // record first
  if (!record(lib, ACTION_REMOVE_BOOK, id, 0)) return LIB_ERR_NOMEM;

  // neither call allocates memory
  graph_remove_vertex(lib->graph, id);
  bst_remove(lib->tree, id);
  return LIB_OK;
}

const Book *library_find_book(const Library *lib, int id) {
  if (lib == NULL) return NULL;
  return bst_search(lib->tree, id);
}

// edges

int library_add_edge(Library *lib, int a, int b) {
  if (lib == NULL) return LIB_ERR_INVALID;

  int rc = graph_add_edge(lib->graph, a, b);
  if (rc != GRAPH_OK) return from_graph(rc);

  if (!record(lib, ACTION_ADD_EDGE, a, b)) {
    graph_remove_edge(lib->graph, a, b);      // undo
    return LIB_ERR_NOMEM;
  }
  return LIB_OK;
}

int library_remove_edge(Library *lib, int a, int b) {
  if (lib == NULL) return LIB_ERR_INVALID;

  int rc = graph_remove_edge(lib->graph, a, b);
  if (rc != GRAPH_OK) return from_graph(rc);

  if (!record(lib, ACTION_REMOVE_EDGE, a, b)) {
    graph_add_edge(lib->graph, a, b);
    return LIB_ERR_NOMEM;
  }
  return LIB_OK;
}

int library_has_edge(const Library *lib, int a, int b) {
  if (lib == NULL) return LIB_ERR_INVALID;
  return from_graph(graph_has_edge(lib->graph, a, b));
}

// queries

int library_reachable(const Library *lib , int origin, List **out) {
  if (lib == NULL) return LIB_ERR_INVALID;
  return from_graph(graph_bfs(lib->graph, origin, out));
}

int library_has_path(const Library *lib, int a, int b) {
  if (lib == NULL) return LIB_ERR_INVALID;
  return from_graph(graph_has_path(lib->graph, a, b));
}

// consistency check

typedef struct {
  const Graph *graph;
  int missing;
} CheckCtx;

static void check_visitor(const Book *book, void *ctx) {
  CheckCtx *c = ctx;
  if (!graph_has_vertex(c->graph, book->id)) c->missing++;
}

int library_is_consistent(const Library *lib) {
  if (lib == NULL) return 0;
  CheckCtx c = { lib->graph, 0 };
  bst_inorder(lib->tree, check_visitor, &c);
  return c.missing == 0 &&
         bst_size(lib->tree) == graph_vertex_count(lib->graph);
}

