/* APPLE LOCAL folding of anon union 6120295 */
/* { dg-do run } */

struct foo { int a; };
struct bar { int b; };

int main(int argc, char* argv[]) {
  union {
    const foo *pfoo;
    const bar *pbar;
  };

  // test that the two pointers occupy the same space on the stack
  if ((void *)&pfoo != (void *)&pbar)
    return 1;
}
