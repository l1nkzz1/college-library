#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorting.h"

static Book mk(int id, const char *title) {
  Book b;
  b.id = id;
  snprintf(b.title, sizeof b.title, "%s", title);
  snprintf(b.author, sizeof b.author, "Author");
  b.year = 2000;
  return b;
}

static void print_arr(const char *label, const Book *a, int n) {
  printf("%-22s:", label);
  for (int i = 0; i < n; i++) printf(" [%d %s]", a[i].id, a[i].title);
  printf("\n");
}

static int is_sorted(const Book *a, int n) {
  for (int i = 1; i < n; i++)
     if (compare_books(&a[i - 1], &a[i]) > 0) return 0;
  return 1;
}

static void run(const char *label, Book *a, int n) {
  print_arr(label, a, n);
  insertion_sort_books(a, n);
  print_arr("  sorted", a, n);
  printf("  %s\n", is_sorted(a, n) ? "OK" : " FAIL");
}

static void ids_visitor(const Book *b, void *ctx) {
  int *sum = ctx;
  *sum += b->id;
}

int main(void) {
  // unsorted
  Book u[] = { mk(1,"Dune"), mk(2,"Anna"), mk(3,"Zebra"), mk(4,"Moby") };
  run("unsorted", u, 4);

  // already sorted
  Book s[] = { mk(1, "A"), mk(2, "B"), mk(3, "C")};
  run("already sorted", s, 3);

  // reversed
  Book r[] = { mk(3, "C"), mk(2, "B"), mk(1, "A")};
  run("reversed", r, 3);

  // same title ordered by id
  Book t[] = { mk(9, "Same"), mk(3, "Same"), mk(5, "Same"), mk(1, "Alpha")};
  run("title", t, 4);
  printf(" order: %s\n",
         (t[1].id == 3 && t[2].id == 5 && t[3].id == 9) ? "OK" : " FAIL");

  // uppercase comes before lowercase in strcmp
  Book c[] = { mk(1, "arte"), mk(2, "Arte"), mk(4, "zebra"), mk(5, "Zebra") };
  run("case sensitive", c, 4);
  printf(" uppercase first: %s\n",
         (strcmp(c[0].title,"Arte")==0 && strcmp(c[1].title,"Zebra")==0) ? "OK" : "FAIL");

  // empty nd simple
  insertion_sort_books(NULL, 0);
  Book one[] = { mk(1, "Only") };
  run("single", one, 1);

  // sorted copy from BST = BST must stay intact
  BST *tree = bst_create();
  int ids[] = {50, 30, 70, 20, 40};
  const char *titles[] = {"Echo", "Alpha", "Delta", "Charlie", "Bravo"};
  for (int i = 0; i < 5; i++) bst_insert(tree, mk(ids[i], titles[i]));

  int sum_before = 0, sum_after = 0;
  bst_inorder(tree, ids_visitor, &sum_before);

  Book *copy = NULL;
  int n = 0;
  int ok = sorting_sorted_copy(tree, &copy, &n);
  printf("sorted_copy rc=%d n=%d\n", ok, n);
  print_arr("  from BST", copy, n);
  printf("  %s\n", (is_sorted(copy, n) && n == 5) ? "OK" : " FAIL");

  // mutating the copy must not affect the BST (independent values) 
  copy[0].id = 9999;
  free(copy);

  bst_inorder(tree, ids_visitor, &sum_after);
  printf("BST intact: %s (size=%d)\n",
         (sum_before == sum_after && bst_size(tree) == 5 && bst_search(tree, 30)) ? "OK" : "FAIL",
          bst_size(tree));

  // empty tree
  BST *empty = bst_create();
  ok = sorting_sorted_copy(empty, &copy, &n);
  printf("empty tree: rc=%d n=%d copy=%p %s\n", ok, n, (void *)copy,
         (ok == 1 && n == 0 && copy == NULL) ? "OK" : "FAIL");

  bst_free(empty);
  bst_free(tree);
  return 0;
}

