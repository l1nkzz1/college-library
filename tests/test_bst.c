#include <stdio.h>
#include "bst.h"

static Book make_book(int id) {
  Book b;
  b.id = id;
  snprintf(b.title, sizeof b.title, "Title %d", id);
  snprintf(b.author, sizeof b.author, "Author %d", id);
  b.year = 2000;
  return b;
}

static void print_id(const Book *b, void *ctx) {
  (void)ctx;
  printf("%d ", b->id);
}

static void show(const BST *t, const char *label) {
  printf("%-28s: ", label);
  bst_inorder(t, print_id, NULL);
  printf("(size=%d)\n", bst_size(t));
}

int main(void) {
  BST *t = bst_create();
  int ids[] = {50, 30, 70, 20, 40, 60, 80, 35, 65};
  int n = (int)(sizeof ids / sizeof ids[0]);

  // empty tree
  printf("min on empty: %p\n", (const void *)bst_min(t));
  printf("remove on empty: %d\n", bst_remove(t, 10));

  for (int i = 0; i < n; i++) bst_insert(t, make_book(ids[i]));
  show(t, "after inserts");

  // duplicate and invalid
  printf("insert duplicate 50: %d\n", bst_insert(t, make_book(50)));
  printf("insert id 0: %d\n", bst_insert(t, make_book(0)));
  printf("insert id -3: %d\n", bst_insert(t, make_book(-3)));
  show(t, "unchanged");

  // search
  printf("search 65: %s\n", bst_search(t, 65) ? "found" : "absent");
  printf("search 99: %s\n", bst_search(t, 99) ? "found" : "absent");

  printf("min=%d max=%d\n", bst_min(t)->id, bst_max(t)->id);

  // remove nonexistent must not change anything
  printf("remove 99: %d\n", bst_remove(t, 99));
  show(t, "after remove 99");

  // leaf
  bst_remove(t, 20);
  show(t, "remove leaf 20");

  // one child 30 has only 40
  bst_remove(t, 30);
  show(t, "remove one-child 30");

  // two children 70 has 60 and 80
  bst_remove(t, 70);
  show(t, "remove two-children 70");

  // root with two children
  bst_remove(t, 50);
  show(t, "remove root 50");

  printf("min=%d max=%d\n", bst_min(t)->id, bst_max(t)->id);

  bst_free(t);
  return 0;
}
