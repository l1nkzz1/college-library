#include <stdio.h>
#include "list.h"

int main(void) {
  List *l = list_create();
  list_print(l);
  printf("size = %d\n", list_size(l));

  list_append(l, 5);
  list_append(l, 1);
  list_append(l, 9);
  list_print(l);
  printf("size = %d\n", list_size(l));

  list_free(l);
  return 0;
}
