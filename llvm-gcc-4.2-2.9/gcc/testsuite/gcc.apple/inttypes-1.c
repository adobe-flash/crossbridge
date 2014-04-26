/* APPLE LOCAL file test of inttypes.h formatter macros */

/* { dg-do compile } */
/* { dg-options "-Wall -W" } */

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>

int main ()
{
    uint32_t x = 12;
    int32_t y = 15;
    printf("x = %" PRIo32 "\n", x);
    printf("x = %" PRIu32 "\n", x);
    printf("x = %" PRIx32 "\n", x);
    printf("x = %" PRIX32 "\n", x);
    printf("y = %" PRId32 "\n", y);
    printf("y = %" PRIi32 "\n", y);
    return 0;
}
