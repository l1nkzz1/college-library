#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

static const char *action_name(ActionType t) {
  switch (t) {
    case ACTION_ADD_BOOK:     return "Add book";
    case ACTION_REMOVE_BOOK:  return "Remove book";
    case ACTION_ADD_EDGE:     return "Add connection";
    case ACTION_REMOVE_EDGE:  return "Remove connection";
  }
  return "Unknown";
}

int stack_push(Stack *s, Action a) {
  if (s == NULL) return 0;

  StackNode *node = malloc(sizeof *node);
  if(node == NULL) return 0;

  node->action = a;
  node->next = s->top;
  s->top = node;
  s->size++;
  return 1;
}

int stack_pop(Stack *s, Action *out) {
  if(s == NULL || s->top == NULL) return 0;

  StackNode *node = s->top;
  if(out != NULL) *out = node->action;
  s->top = node->next;
  free(node);
  s->size--;
  return 1;
}

int stack_peek(const Stack *s, Action *out) {
  if (s == NULL || s->top == NULL) return 0;
  if (out != NULL) *out = s->top->action;
  return 1;
}

int stack_is_empty(const Stack *s){
  return s == NULL || s->top == NULL;
}

int stack_size(const Stack *s) {
  return s ? s->size : 0;
}

void stack_print_last(const Stack *s, int n) {
  if (stack_is_empty(s)) {
    printf("History is empty.\n");
    return;
  }

  int shown = 0;
  for (const StackNode *node = s->top; node != NULL && shown < n; node = node->next){
    shown++;
    printf("%d. %s", shown, action_name(node->action.type));
    if(node->action.id2 != 0)
      printf(" (%d, %d)\n", node->action.id1, node->action.id2);
    else
      printf(" (%d)\n", node->action.id1);
  }
}

void stack_free(Stack *s) {
  if(s == NULL) return;
  StackNode *node = s->top;
  while (node != NULL) {
   StackNode *next = node->next;
   free(node);
   node = next;
  }
  free(s);
}

