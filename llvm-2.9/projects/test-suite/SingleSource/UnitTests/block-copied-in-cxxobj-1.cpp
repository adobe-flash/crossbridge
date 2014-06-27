#if defined(__BLOCKS__) && defined(__clang__)
// rdar://8768050
#include <stdio.h>
#include <stdlib.h>
#include <Block.h>

static int expected_dtors;

class A {
private:
	void *p;
public:
	A() throw() {
	}
	A(const A &a) throw() : p(a.p) {
	}
	A &operator =(const A &a) throw() {
		return *this;
	}
	~A() throw() {
		expected_dtors--;
		if (expected_dtors < 0) {
			abort();
		}
	}
	void m() const throw() {
	}
};

#endif

int
main(void)
{
#if defined(__BLOCKS__) && defined(__clang__)
	A a;

	void (^b)(void) = Block_copy(^{
		a.m();
	});

	b();

        expected_dtors = 3;
	Block_release(b);
#endif
	return 0;
}
