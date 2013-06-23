/* +++Date last modified: 05-Jul-1997 */

/*
**  Functions to maintain an arbitrary length array of bits
*/

#include "bitops.h"

char *alloc_bit_array(size_t bits)
{
      char *set = calloc((bits + CHAR_BIT - 1) / CHAR_BIT, sizeof(char));

      return set;
}

int getbit(char *set, int number)
{
        set += number / CHAR_BIT;
        return (*set & (1 << (number % CHAR_BIT))) != 0;    /* 0 or 1   */
}

void setbit(char *set, int number, int value)
{
        set += number / CHAR_BIT;
        if (value)
                *set |= 1 << (number % CHAR_BIT);           /* set bit  */
        else    *set &= ~(1 << (number % CHAR_BIT));        /* clear bit*/
}

void flipbit(char *set, int number)
{
        set += number / CHAR_BIT;
        *set ^= 1 << (number % CHAR_BIT);                   /* flip bit */
}
