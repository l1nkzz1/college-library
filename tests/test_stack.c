#include <stdio.h>
#include "stack.h"

int main(void) {
  Stack *s = stack_create();
  Action a;

  stack_print_last(s, 5);
  printf("pop on empty: %d\n", stack_pop(s, &a));

  for (int i = 1; i <= 7; i++) {
    Action x = { ACTION_ADD_BOOK, i *  10, 0};
    stack_push(s, x);
  }
  Action e = { ACTION_ADD_EDGE, 10, 20};
  stack_push(s, e);

  stack_print_last(s, 5);
  printf("size after print = %d\n", stack_size(s));

  stack_peek(s, &a);
  printf("peek: type=%d id1=%d id2=%d\n", a.type, a.id1, a.id2);

  stack_pop(s, &a);
  printf("size after pop = %d\n", stack_size(s));

  stack_free(s);
  return 0;
}
