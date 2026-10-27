#include <stdio.h>
#include "graph.h"

static void check(const char *label, int got, int expected) {
    printf("%-34s got=%2d expected=%2d  %s\n",
           label, got, expected, got == expected ? "OK" : "<-- FAIL");
}

static void show_bfs(const Graph *g, int origin) {
    List *l;
    int rc = graph_bfs(g, origin, &l);
    printf("BFS from %d: ", origin);
    if (rc == GRAPH_OK) { list_print(l); list_free(l); }
    else printf("error %d\n", rc);
}

int main(void) {
    Graph *g = graph_create();

    /* ids added out of order, so slot index != id order */
    int ids[] = {500, 100, 300, 200, 400, 900, 700, 800};
    for (int i = 0; i < 8; i++) graph_add_vertex(g, ids[i]);
    check("vertex count", graph_vertex_count(g), 8);
    check("duplicate vertex 100", graph_add_vertex(g, 100), GRAPH_ERR_EXISTS);
    check("invalid vertex 0", graph_add_vertex(g, 0), GRAPH_ERR_INVALID);

    /* edges */
    check("add (500,100)", graph_add_edge(g, 500, 100), GRAPH_OK);
    check("duplicate (100,500)", graph_add_edge(g, 100, 500), GRAPH_ERR_EXISTS);
    check("duplicate (500,100)", graph_add_edge(g, 500, 100), GRAPH_ERR_EXISTS);
    check("loop (300,300)", graph_add_edge(g, 300, 300), GRAPH_ERR_LOOP);
    check("edge with missing id", graph_add_edge(g, 500, 12345), GRAPH_ERR_NOTFOUND);
    check("remove missing edge", graph_remove_edge(g, 300, 200), GRAPH_ERR_NOEDGE);
    graph_add_edge(g, 500, 300);
    graph_add_edge(g, 500, 200);
    graph_add_edge(g, 100, 400);
    graph_add_edge(g, 700, 800);
    check("direct (100,500)", graph_has_edge(g, 100, 500), 1);
    check("direct (500,100)", graph_has_edge(g, 500, 100), 1);
    check("direct (100,300)", graph_has_edge(g, 100, 300), 0);
    check("direct with missing id", graph_has_edge(g, 100, 999), GRAPH_ERR_NOTFOUND);

    // BFS: neighbors in ascending id order 
    show_bfs(g, 500);          /* 500 -> 100 -> 200 -> 300 -> 400 */
    show_bfs(g, 900);          /* 900 (isolated) */
    show_bfs(g, 700);          /* 700 -> 800 (other component) */
    show_bfs(g, 12345);        /* error -4 */

    /* paths */
    check("path 500->400", graph_has_path(g, 500, 400), 1);
    check("path 500->700 (disconnected)", graph_has_path(g, 500, 700), 0);
    check("path 900->900", graph_has_path(g, 900, 900), 1);
    check("path with missing id", graph_has_path(g, 500, 12345), GRAPH_ERR_NOTFOUND);

    /* remove a vertex with several edges */
    check("remove vertex 500", graph_remove_vertex(g, 500), GRAPH_OK);
    check("remove vertex 500 again", graph_remove_vertex(g, 500), GRAPH_ERR_NOTFOUND);
    check("edge to removed vertex", graph_has_edge(g, 100, 500), GRAPH_ERR_NOTFOUND);
    show_bfs(g, 100);          /* 100 -> 400 */
    check("path 100->300 after removal", graph_has_path(g, 100, 300), 0);

    /* re-adding the id must not bring old edges back */
    graph_add_vertex(g, 500);
    show_bfs(g, 500);          /* 500 only */

    graph_free(g);

    /* capacity: 101st vertex rejected */
    g = graph_create();
    int rc = GRAPH_OK;
    for (int id = 1; id <= 100; id++) rc = graph_add_vertex(g, id);
    check("100th vertex", rc, GRAPH_OK);
    check("101st vertex", graph_add_vertex(g, 101), GRAPH_ERR_FULL);
    check("count stays 100", graph_vertex_count(g), 100);
    graph_remove_vertex(g, 50);
    check("slot reused after removal", graph_add_vertex(g, 101), GRAPH_OK);
    graph_free(g);

    return 0;
}