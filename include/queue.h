#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode {
  int value;
  struct QueueNode *next;
} QueueNode;

typedef struct {
  QueueNode *front;
  QueueNode *rear;
  int size;
} Queue;

Queue *queue_create(void);
int    queue_enqueue(Queue *q, int value);
int    queue_dequeue(Queue *q, int *out);
int    queue_is_empty(const Queue *q);
int    queue_size(const Queue *q);
void   queue_free(Queue *q);

#endif
