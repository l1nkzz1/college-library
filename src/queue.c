#include <stdlib.h>
#include "queue.h"

Queue *queue_create(void) {
  Queue *q = malloc(sizeof *q);
  if(q == NULL) return NULL;
  q->front = NULL;
  q->rear = NULL;
  q->size = 0;
  return q;
}

int queue_enqueue(Queue *q, int value) {
  if (q == NULL) return 0;

  QueueNode *node = malloc(sizeof *node);
  if (node == NULL) return 0;
  node->value = value;
  node->next = NULL;

  if (q->rear == NULL) {
    q->front = node;
  } else {
    q->rear->next = node;
  }

  q->rear = node;
  q->size++;
  return 1;
}

int queue_dequeue(Queue *q, int *out) {
  if (q == NULL || q->front == NULL) return 0;

  QueueNode *node = q->front;
  if (out != NULL) *out = node->value;
  q->front = node->next;
  if(q->front == NULL)              // queue became empty
     q->rear = NULL;
  free(node);
  q->size--;
  return 1;
}

int queue_is_empty(const Queue *q) {
  return q == NULL || q->front == NULL;
}

int queue_size(const Queue *q) {
  return q ? q->size : 0;
}

void queue_free(Queue *q) {
  if (q == NULL) return;
  QueueNode *n = q->front;
  while (n != NULL) {
    QueueNode *next = n->next;
    free(n);
    n = next;
  }
  free(q);
}
