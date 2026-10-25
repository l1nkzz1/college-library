#ifndef STACK_H
#define STACK_H

typedef enum {
  ACTION_ADD_BOOK,
  ACTION_REMOVE_BOOK,
  ACTION_ADD_EDGE,
  ACTION_REMOVE_EDGE
} ActionType;

typedef struct {
  ActionType type;
  int id1;
  int id2;            // O when the action involves a single ID
} Action;

typedef struct StackNode {
  Action action;
  struct StackNode *next;
} StackNode;

typedef struct {
  StackNode *top;
  int size;
} Stack;

Stack *stack_create(void);
int    stack_push(Stack *s, Action a);
int    stack_pop(Stack *s, Action *out);
int    stack_peek(const Stack *s, Action *out);
int    stack_is_empty(const Stack *s);
void   stack_print_last(const Stack *s, int n);
void   stack_free(Stack *s);

#endif
