// Test that the nothrow optimization works properly.
// { dg-do compile }
// { dg-options "-O -fdump-tree-optimized" }
// LLVM LOCAL fdump not supported
// { dg-require-fdump "" }

extern void blah() throw();

int i, j, k;

int main()
{
  try
    {
      ++i;
      blah();
      ++j;
    }
  catch (...)
    {
      return -42;
    }
}

// The catch block should be optimized away.
// { dg-final { scan-tree-dump-times "-42" 0 "optimized" } }
// { dg-final { cleanup-tree-dump "optimized" } }
