/* { dg-options "-fpreprocessed" } */
/* { dg-compile } */

# 1 "/tmp/x.cc"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "/tmp/x.cc"
struct foo {
    int x;
} f[] = {
# 1 "/tmp/yy" 1
{1}
# 5 "/tmp/x.cc" 2
    , {0}
};
