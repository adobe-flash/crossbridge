/* APPLE LOCAL file 4101687 */
/* { dg-do run } */
/* { dg-options "-Os" } */
#include <stdlib.h>

void
fill(char *buf)
{
    int i;

    for (i = 0; i < 16; i++) {
        buf[i+i] = 2;
        buf[i+i+1] = 3;
    }
    buf[i+i] = '\0';
}

int main()
{
    char buf[33];

    fill(buf);
    if (buf[16] == '\0')
      abort ();
    return 0;
}
