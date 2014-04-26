// { dg-do run  }
extern "C" int printf(const char *, ...);
extern "C" const char *getenv(const char *);

class T {
  int i;
public:
  T() {
    i = 1;
    /* APPLE LOCAL default to Wformat-security 5764921 */
    printf("T() at %p\n", (void*)this);
  }
  T(const T& o) {
    i = o.i;
    /* APPLE LOCAL default to Wformat-security 5764921 */
    printf("T(const T&) at %p <-- %p\n", (void*)this, (void*)&o);
  }
  T operator +(const T& o) {
    T r;
    r.i = this->i + o.i;
    return r;
  }
  operator int () {
    return i;
  }
  /* APPLE LOCAL default to Wformat-security 5764921 */
  ~T() { printf("~T() at %p\n", (void*)this); }
} s, b;

int foo() { return getenv("TEST") == 0; }

int main() {
  int i = foo() ? s+b : s;
  return i != 2;
}
