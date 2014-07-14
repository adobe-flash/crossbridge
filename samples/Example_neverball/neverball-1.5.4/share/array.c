#include <stdlib.h>
#include <assert.h>

#include "array.h"
#include "common.h"

struct array
{
    unsigned char *data;

    int elem_num;
    int elem_len;
    int data_len;
};

Array array_new(int elem_len)
{
    Array a;

    assert(elem_len > 0);

    if ((a = malloc(sizeof (*a))))
    {
        a->data = malloc(elem_len);

        a->elem_num = 0;
        a->elem_len = elem_len;
        a->data_len = elem_len;
    }

    return a;
}

void array_free(Array a)
{
    assert(a);

    free(a->data);
    free(a);
}

void *array_add(Array a)
{
    assert(a);

    if ((a->elem_num + 1) * a->elem_len > a->data_len)
        a->data = realloc(a->data, (a->data_len *= 2));

    return &a->data[a->elem_num++ * a->elem_len];
}

void array_del(Array a)
{
    assert(a);
    assert(a->elem_num > 0);

    if (a->elem_num-- * a->elem_len < a->data_len / 4)
        a->data = realloc(a->data, (a->data_len /= 4));
}

void *array_get(Array a, int i)
{
    assert(a);
    assert(i >= 0 && i < a->elem_num);

    return &a->data[i * a->elem_len];
}

void *array_rnd(Array a)
{
    assert(a);

    return a->elem_num ? array_get(a, rand_between(0, a->elem_num - 1)) : NULL;
}

int array_len(Array a)
{
    assert(a);

    return a->elem_num;
}

void array_sort(Array a, int (*cmp)(const void *, const void *))
{
    assert(a);

    qsort(a->data, a->elem_num, a->elem_len, cmp);
}
