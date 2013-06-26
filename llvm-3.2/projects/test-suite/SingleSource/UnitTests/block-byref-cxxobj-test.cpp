// rdar://8594790
#if defined(__BLOCKS__) && defined(__clang__)
#include <Block.h>
extern "C" void abort();
static int count=0;
class A {
public:
        int x;
        A(const A &o) { ++count; x = o.x; 
	  if (this == &o)
            abort();
        }
        A &operator =(const A &o) { x = o.x; return *this; }
        A() : x(100) { ++count; }
        ~A();
        void hello() const { 
          if (x != 100) 
            abort();
        }
};

A::~A() { 
  --count; 
  if (x != 100)
    abort();
  x = 0; 
}
#endif

int
main()
{
#if defined(__BLOCKS__) && defined(__clang__)
  if (!count) {
        __block A a;
        A ca;
        void (^b)(void) = Block_copy(^{ a.hello(); ca.hello(); });
        b();
        Block_release(b);
  }
  if (count)
    abort();
#endif
  return 0;
}

