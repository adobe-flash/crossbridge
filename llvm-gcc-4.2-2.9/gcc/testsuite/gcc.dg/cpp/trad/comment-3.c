/* Test we don't accept C++ comments.  */

/* { dg-do preprocess } */
/* APPLE LOCAL -Wextra-tokens 2001-08-01 --sts */
/* { dg-options "-traditional-cpp -Wextra-tokens" } */

#if 0
#endif //  /* { dg-warning "extra tokens" } */
