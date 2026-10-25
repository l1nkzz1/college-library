#ifndef LIST_H
#define LIST_H

typedef struct ListNode {
  int id;
  struct ListNode *next;
} ListNode;

typedef struct {
  ListNode *head;
  ListNode *tail;
  int size;
} List;

List *list_create(void);
int   list_append(List *l, int id);
int   list_size(const List *l);
void  list_print(const List *l);
void  list_free(List *l);

#endif
