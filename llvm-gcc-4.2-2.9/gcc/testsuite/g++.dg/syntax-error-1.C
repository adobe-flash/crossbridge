/* APPLE LOCAL file 4185810 */
/* Recover gracefully from a syntax error.  */

void function(void)
{
        if( 1 )
        {
        else /* { dg-error "expected .\}. before .else." } */
        {
        }
}

