/* APPLE LOCAL file radar 6035389 */
/* { dg-options "-Werror -fblocks" } */

int print(const char *);

int main()
{
  void (^b)(void);

  __sync_bool_compare_and_swap(&b, (void*)0, ^{ print("hello\n"); });
  return 0;
};
