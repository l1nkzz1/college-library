#include <stdio.h>
#include <stdlib.h>
#include <list.h>

List *list_create(void) {
  List *l = malloc(sizeof *l);
  if (l == NULL) return NULL;
  l->head = NULL;
  l->tail = NULL;
  l->size = 0;
  return l;
}

int list_append(List *l, int id) {
  if(l == NULL) return 0;

  ListNode *node = malloc(sizeof *node);
  if (node == NULL) return 0;
  node->id = id;
  node->next = NULL;

  if (l->tail == NULL){
    l->head = node;
  } else {
    l->tail->next = node;
  }

  l->tail = node;
  l->size++;
  return 1;
}

int list_size(const List *l){
  return l ? l->size : 0;
}

void list_print(const List *l) {
  if (l == NULL || l->head == NULL) {
    printf("empty\n");
    return;
  }
  for (const ListNode *n = l->head; n != NULL; n = n->next) {
    printf("%d", n->id);
    if (n->next != NULL) printf (" -> ");
  } 
  printf("\n");
}

void list_free(List *l) {
  if (l == NULL) return;
  ListNode *n = l->head;
  while (n != NULL) {
    ListNode *next = n->next;
    free(n);
    n = next;
  }
  free(l);
}
