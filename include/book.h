#ifndef BOOK_H
#define BOOK_H

#define TEXT_SIZE 80
#define YEAR_MIN  1
#define YEAR_MAX  2026

typedef struct {
  int id;
  char title[TEXT_SIZE];
  char author[TEXT_SIZE];
  int year;
} Book;

#endif
