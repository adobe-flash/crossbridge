/* { dg-options "" } */

#warning Don't? /* { dg-warning "Don't?" } */
#if 0
#error Don't? /* { dg-bogus "Don't?" } */
#endif
#if 1
#error Don't? /* { dg-error "Don't?" } */
#endif
