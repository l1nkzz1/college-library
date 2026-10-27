#include <stdlib.h>
#include "graph.h"
#include "queue.h"

/* ---------- internal helpers ---------- */

static int find_index(const Graph *g, int id) {
    if (g == NULL || id <= 0) return -1;
    for (int i = 0; i < GRAPH_MAX; i++)
        if (g->used[i] && g->ids[i] == id) return i;
    return -1;
}

/* Fills order[] with the used slots sorted by ascending id
   (insertion sort). Returns how many there are. */
static int slots_by_id(const Graph *g, int order[GRAPH_MAX]) {
    int n = 0;
    for (int i = 0; i < GRAPH_MAX; i++) {
        if (!g->used[i]) continue;
        int j = n++;
        while (j > 0 && g->ids[order[j - 1]] > g->ids[i]) {
            order[j] = order[j - 1];
            j--;
        }
        order[j] = i;
    }
    return n;
}

/* ---------- create / free ---------- */

Graph *graph_create(void) {
    return calloc(1, sizeof(Graph));   /* everything starts at zero */
}

void graph_free(Graph *g) {
    free(g);
}

/* ---------- vertices ---------- */

int graph_add_vertex(Graph *g, int id) {
    if (g == NULL || id <= 0) return GRAPH_ERR_INVALID;
    if (find_index(g, id) >= 0) return GRAPH_ERR_EXISTS;

    for (int i = 0; i < GRAPH_MAX; i++) {
        if (!g->used[i]) {
            g->used[i] = 1;
            g->ids[i] = id;
            g->count++;
            return GRAPH_OK;      /* row/column are already zero */
        }
    }
    return GRAPH_ERR_FULL;
}

int graph_remove_vertex(Graph *g, int id) {
    int idx = find_index(g, id);
    if (idx < 0) return GRAPH_ERR_NOTFOUND;

    for (int k = 0; k < GRAPH_MAX; k++) {
        g->adj[idx][k] = 0;       /* row */
        g->adj[k][idx] = 0;       /* column */
    }
    g->used[idx] = 0;
    g->ids[idx] = 0;
    g->count--;
    return GRAPH_OK;
}

int graph_has_vertex(const Graph *g, int id) {
    return find_index(g, id) >= 0;
}

int graph_vertex_count(const Graph *g) {
    return g ? g->count : 0;
}

/* ---------- edges ---------- */

int graph_add_edge(Graph *g, int a, int b) {
    int i = find_index(g, a);
    int j = find_index(g, b);
    if (i < 0 || j < 0) return GRAPH_ERR_NOTFOUND;
    if (i == j) return GRAPH_ERR_LOOP;
    if (g->adj[i][j]) return GRAPH_ERR_EXISTS;   /* (A,B) or (B,A) */

    g->adj[i][j] = 1;
    g->adj[j][i] = 1;
    return GRAPH_OK;
}

int graph_remove_edge(Graph *g, int a, int b) {
    int i = find_index(g, a);
    int j = find_index(g, b);
    if (i < 0 || j < 0) return GRAPH_ERR_NOTFOUND;
    if (i == j || !g->adj[i][j]) return GRAPH_ERR_NOEDGE;

    g->adj[i][j] = 0;
    g->adj[j][i] = 0;
    return GRAPH_OK;
}

int graph_has_edge(const Graph *g, int a, int b) {
    int i = find_index(g, a);
    int j = find_index(g, b);
    if (i < 0 || j < 0) return GRAPH_ERR_NOTFOUND;
    return g->adj[i][j] ? 1 : 0;
}

/* ---------- BFS ---------- */

int graph_bfs(const Graph *g, int origin, List **out) {
    if (g == NULL || out == NULL) return GRAPH_ERR_INVALID;
    *out = NULL;

    int start = find_index(g, origin);
    if (start < 0) return GRAPH_ERR_NOTFOUND;

    int order[GRAPH_MAX];
    int n = slots_by_id(g, order);

    int visited[GRAPH_MAX] = {0};
    List  *list  = list_create();
    Queue *queue = queue_create();
    if (list == NULL || queue == NULL) goto fail;

    visited[start] = 1;                       /* marked when entering the queue */
    if (!queue_enqueue(queue, start) || !list_append(list, origin))
        goto fail;

    while (!queue_is_empty(queue)) {
        int cur;
        queue_dequeue(queue, &cur);

        for (int k = 0; k < n; k++) {         /* ascending id order */
            int j = order[k];
            if (g->adj[cur][j] && !visited[j]) {
                visited[j] = 1;
                if (!queue_enqueue(queue, j) || !list_append(list, g->ids[j]))
                    goto fail;
            }
        }
    }

    queue_free(queue);
    *out = list;
    return GRAPH_OK;

fail:
    queue_free(queue);
    list_free(list);
    return GRAPH_ERR_NOMEM;
}

int graph_has_path(const Graph *g, int a, int b) {
    if (g == NULL) return GRAPH_ERR_INVALID;
    if (find_index(g, a) < 0 || find_index(g, b) < 0)
        return GRAPH_ERR_NOTFOUND;            /* absent id is not "false" */

    List *reach;
    int rc = graph_bfs(g, a, &reach);
    if (rc != GRAPH_OK) return rc;

    int found = 0;
    for (const ListNode *n = reach->head; n != NULL; n = n->next) {
        if (n->id == b) { found = 1; break; }
    }
    list_free(reach);
    return found;
}