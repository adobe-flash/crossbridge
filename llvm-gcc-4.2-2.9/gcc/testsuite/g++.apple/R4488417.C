/* { dg-options "-fpreprocessed" } */
/* { dg-compile } */

# 1 "a.cc"
struct my_option
{
  const char *name;
  int id;
};

static struct my_option ml[] =
  {
    {"one", 1},
    {"two", 2},
    {"three", 3},
# 1 "blah.h" 1
# 42 "a.cc" 2
    {"four",4}
  };

