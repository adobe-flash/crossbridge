/* APPLE LOCAL begin radar 4204796 */
/* { dg-options "-fobjc-exceptions -fnext-runtime" } */
/* { dg-do compile } */

typedef unsigned char uint8_t;
typedef uint8_t foo[24];

void thingy(foo a)
{
}

int main()
{
    foo bar;

    @try {
    } 
    @finally {
    }

    thingy(bar);

    return 0;
}
/* APPLE LOCAL end radar 4204796 */
