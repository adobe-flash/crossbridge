#if defined(__BLOCKS__) && defined(__clang__)
// rdar://8594790
#include <Block.h>

extern "C" void abort();

int count = 100;
int no=1;

struct S {
  S() { no = 0; };
  ~S() { if (no != 0) abort(); }
};

static S s1;

class A {
public:
        int x;
        A(const A &o) { ++no; x = o.x; }
        A &operator =(const A &o) { x = o.x; return *this; }
        A() : x(count++) { ++no; }
        virtual ~A() { --no; }
        void hello() const { }
};
#endif

int
main()
{
#if defined(__BLOCKS__) && defined(__clang__)
        A a;
        A a1;
        A a2;
        void (^b)(void) = ^{ a.hello(); a1.hello(); a2.hello(); };
        b();

        void (^c)(void) = Block_copy(^{ a.hello(); a1.hello(); a2.hello(); });
        c();
	if (no != 12)
	  abort();
        Block_release(c);
	if (no != 9)
	  abort();
#endif
        return 0;
}

