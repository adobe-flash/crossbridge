/* APPLE LOCAL file mainline aligned functions 5933878 */
void func(void) __attribute__((aligned(256)));

void func(void)
{
}

int main()
{
  /* ARM thumb functions set the low bit of the address. */
#if defined(__arm__) && defined(__thumb__)
  if (((long)func & 0xFF) != 1)
#else
  if (((long)func & 0xFF) != 0)
#endif
    abort ();
  if (__alignof__(func) != 256)
    abort ();
  return 0;
}
