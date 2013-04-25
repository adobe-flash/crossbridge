extern "C" void abort (void);
extern "C" int printf(const char*, ...);
struct foo { unsigned long long x; } x;
int main() {
  /* printf("%d\n", __alignof__(x.x)); */
  if (__alignof__ (x) != __alignof__ (x.x))
    abort ();
}
