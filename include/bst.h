#ifndef BST_H
#define BST_H

#include "book.h"

#define BST_OK         1
#define BST_DUPLICATE  0
#define BST_INVALID   -1
#define BST_NOMEM     -2

typedef struct BSTNode {
  Book book;
  struct BSTNode *left;
  struct BSTNode *right;
} BSTNode;

typedef struct {
  BSTNode *root;
  int size;
} BST;

typedef void (*BookVisitor)(const Book *book, void *ctx);

BST        *bst_create(void);
int         bst_insert(BST *t, Book book);
const Book *bst_search(const BST *t, int id);
int         bst_remove(BST *t, int id);
void        bst_inorder(const BST *t, BookVisitor visit, void *ctx);
const Book *bst_min(const BST *t);
const Book *bst_max(const BST *t);
int         bst_size(const BST *t);
void        bst_free(BST *t);

#endif

