#include <stdlib.h>
#include <bst.h>

BST *bst_create(void) {
  BST *t = malloc(sizeof *t);
  if (t == NULL) return NULL;
  t->root = NULL;
  t->size = 0;
  return t;
}

// insert

static BSTNode *insert_rec(BSTNode *node, Book book, int *status) {
  if (node == NULL) {
    BSTNode *n = malloc(sizeof *n);
    if (n == NULL) {
      *status = BST_NOMEM;
      return NULL;
    }
    n->book  = book;
    n->left  = NULL;
    n->right = NULL;
    *status = BST_OK;
    return n;
  }
  if (book.id < node->book.id)
      node->left = insert_rec(node->left, book, status);
  else if (book.id > node->book.id)
      node->right = insert_rec(node->right, book, status);
  else
      *status = BST_DUPLICATE;
  return node;
}

int bst_insert(BST *t, Book book) {
  if (t == NULL || book.id <= 0) return BST_INVALID;
  int status = BST_NOMEM;
  t->root = insert_rec(t->root, book, &status);
  if (status == BST_OK) t->size++;
  return status;
}

// search

const Book *bst_search(const BST *t, int id) {
  if (t == NULL) return NULL;
  const BSTNode *n = t->root;
  while(n != NULL) {
    if (id < n->book.id)      n = n->left;
    else if (id > n->book.id) n = n->right;
    else                      return &n->book;
  }
  return NULL;
}

// min & max

static BSTNode *leftmost(BSTNode *n) {
  while (n != NULL && n->left != NULL) n = n->left;
  return n;
}

const Book *bst_min(const BST *t) {
  if (t == NULL || t->root == NULL) return NULL;
  return &leftmost(t->root)->book;
}

const Book *bst_max(const BST *t) {
  if (t == NULL || t->root == NULL) return NULL;
  const BSTNode *n = t->root;
  while (n->right != NULL) n = n->right;
  return &n->book;
}

// remove

static BSTNode *remove_rec(BSTNode *node, int id, int *removed) {
  if (node == NULL) return NULL;

  if (id < node->book.id) {
     node->left = remove_rec(node->left, id, removed);
  } else if (id > node->book.id) {
     node->right = remove_rec(node->right, id, removed);
  } else {
    *removed = 1;

    if (node->left == NULL) {
      BSTNode *child = node->right;
      free(node);
      return child;
    }
    if (node->right == NULL) {
      BSTNode *child = node->left;
      free(node);
      return child;
    }

    // 2 children: replace with inorder sucessor
    BSTNode *succ = leftmost(node->right);
    node->book = succ->book;
    int dummy = 0;
    node->right = remove_rec(node->right, succ->book.id, &dummy);
  }
  return node;
}

int bst_remove(BST *t, int id) {
    if (t == NULL) return 0;
    int removed = 0;
    t->root = remove_rec(t->root, id, &removed);
    if (removed)  t->size--;
    return removed;
}

// inorder
static void inorder_rec(const BSTNode *n, BookVisitor visit, void *ctx) {
  if (n == NULL) return;
  inorder_rec(n->left, visit, ctx);
  visit(&n->book, ctx);
  inorder_rec(n->right, visit, ctx);
}

void bst_inorder(const BST *t, BookVisitor visit, void *ctx) {
  if (t == NULL || visit == NULL) return;
  inorder_rec(t->root, visit, ctx);
}

int bst_size(const BST *t) {
  return t ? t->size : 0;
}

// free

static void free_rec(BSTNode *n) {
  if (n == NULL) return;
  free_rec(n->left);
  free_rec(n->right);
  free(n);
}

void bst_free(BST *t) {
  if (t == NULL) return;
  free_rec(t->root);
  free(t);
}
