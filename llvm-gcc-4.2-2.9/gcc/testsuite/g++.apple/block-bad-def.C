/* APPLE LOCAL file radar 5985368 */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

int main(void)
{
        void (^b)(void) { }; /* { dg-error "expected primary-expression" } */
                            /* { dg-error "expected" "" { target *-*-* } 7 } */

        return 0;
}
