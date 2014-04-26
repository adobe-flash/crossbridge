/* { dg-do run } */
/* { dg-options "-fwritable-strings" } */

/*#include <stdio.h>*/
void foo( void *dst)
{
        char *ucdst;

        ucdst = (char *)dst;
        ucdst++;
        *ucdst = 'X';
}

int main()
{
        char *a = "Hello";
        foo ((void *) a);
        return 0;
}
