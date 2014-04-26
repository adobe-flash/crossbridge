// { dg-do run  }
// prms-id: 3579

extern "C" int printf(const char *, ...);

int num_x;

class Y {
public:
  /* APPLE LOCAL default to Wformat-security 5764921 */
  Y () { printf("Y()            this: %p\n", (void*)this); }
  /* APPLE LOCAL default to Wformat-security 5764921 */
  ~Y () { printf("~Y()           this: %p\n", (void*)this); }
};

class X {
public:
  X () {
    ++num_x;
    /* APPLE LOCAL default to Wformat-security 5764921 */
    printf("X()            this: %p\n", (void*)this);
    Y y;
    *this = (X) y;
  }

  /* APPLE LOCAL default to Wformat-security 5764921 */
  X (const Y & yy) { printf("X(const Y&)    this: %p\n", (void*)this); ++num_x; }
  X & operator = (const X & xx) {
    /* APPLE LOCAL default to Wformat-security 5764921 */
    printf("X.op=(X&)      this: %p\n", (void*)this);
    return *this;
  }

  /* APPLE LOCAL default to Wformat-security 5764921 */
  ~X () { printf("~X()           this: %p\n", (void*)this); --num_x; }
};

int main (int, char **) {
    { X anX; }
    if (num_x) {
      printf("FAIL\n");
      return 1;
    }
    printf("PASS\n");
    return 0;
}
