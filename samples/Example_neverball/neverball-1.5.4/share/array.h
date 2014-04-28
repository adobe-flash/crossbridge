#ifndef ARRAY_H
#define ARRAY_H

typedef struct array *Array;

Array array_new(int);
void  array_free(Array);

void *array_add(Array);
void  array_del(Array);
void *array_get(Array, int);
void *array_rnd(Array);
int   array_len(Array);

void  array_sort(Array, int (*cmp)(const void *, const void *));

#endif
