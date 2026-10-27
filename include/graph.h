#ifndef GRAPH_H
#define GRAPH_H

#include "list.h"

#define GRAPH_MAX 100

#define GRAPH_OK             1
#define GRAPH_ERR_INVALID   -1
#define GRAPH_ERR_FULL      -2
#define GRAPH_ERR_EXISTS    -3
#define GRAPH_ERR_NOTFOUND  -4
#define GRAPH_ERR_LOOP      -5
#define GRAPH_ERR_NOEDGE    -6
#define GRAPH_ERR_NOMEM     -7

typedef struct {
  int ids[GRAPH_MAX];
  int used[GRAPH_MAX];
  unsigned char adj[GRAPH_MAX][GRAPH_MAX];
  int count;
} Graph;

Graph *graph_create(void);
void   graph_free(Graph *g);

int graph_add_vertex(Graph *g, int id);
int graph_remove_vertex(Graph *g, int id);
int graph_has_vertex(const Graph *g, int id);
int graph_vertex_count(const Graph *g);

int graph_add_edge(Graph *g, int a, int b);
int graph_remove_edge(Graph *g, int a, int b);
int graph_has_edge(const Graph *g, int a, int b);

// On GRAPH_OK , *out holds the reachable ids in discovery order
// origin first. The caller must call list_free(*out)

int graph_bfs(const Graph *g, int origin, List **out);
int graph_has_path(const Graph *g, int a, int b);      // 1, 0 or error

#endif
