#include <stdio.h>
#include "queue.h"

int main(void) {
  Queue *q = queue_create();
  int v;

  printf("empty: %d\n", queue_is_empty(q));
  printf("dequeue on empty: %d\n", queue_dequeue(q, &v));

  queue_enqueue(q, 10);
  queue_enqueue(q, 20);
  queue_enqueue(q, 30);
  printf("size = %d\n", queue_size(q));             // 3

  while (queue_dequeue(q, &v))
      printf("%d ", v);
  printf("\n");

  // reuse after emptying: checks that rear was reset
  queue_enqueue(q, 99);
  queue_dequeue(q, &v);
  printf("after reuse: %d, empty: %d\n", v, queue_is_empty(q));

  queue_free(q);
  return 0;
}
