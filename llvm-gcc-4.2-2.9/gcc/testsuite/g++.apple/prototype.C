/* APPLE LOCAL file warn missing prototype 6261539 */
/* Radar 6261539 */
/* { dg-options "-Wmissing-prototypes" } */
void foo() { }		/* { dg-warning "no previous prototype" } */
void foo(int i) { }	/* { dg-warning "no previous prototype" } */
void bar();
void bar() { }
void bar1();
void bar1(int i);
void bar1(int i) { }
void bar1(float) { }	/* { dg-warning "no previous prototype" } */
extern "C" void bar1(char);
extern "C" void bar1(char) { }	/* { dg-error "previous declaration" } */
extern "C" void bar1(short);	/* { dg-error "conflicts with" } */
extern "C" void bar1(short) { }
extern "C" void bar2(char);
extern "C" void bar2(char) { }	/* { dg-error "previous declaration" } */
extern "C" void bar2(short) { }	/* { dg-error "conflicts with" } */
extern "C" void bar3(char);
extern "C" void bar3(char) { }	/* { dg-error "previous declaration" } */
extern "C" void bar3(short) { }	/* { dg-error "conflicts with" } */
struct beef { };
void beef() { }		/* { dg-warning "no previous prototype" } */
void dead();
namespace {
  void dead() { }
}
void dead4();
namespace A {
  void dead4() { }	/* { dg-warning "no previous prototype" } */
}
void dead1();
namespace A {
  void dead1(int);
  void dead1() { }	/* { dg-warning "no previous prototype" } */
}
void dead2();		/* { dg-error "old declaration" } */
int dead2() { }		/* { dg-error "new declaration" } */
struct undef;
undef meat;		/* { dg-error "incomplete type and cannot be defined" } */
double meat() { }
static void local() { }
int main() { }
void exit(float);
void exit(int e) { a: goto a; }	/* { dg-warning "no previous prototype" } */
void dead3(float);
int dead3(int);		/* { dg-error "old declaration" } */
void dead3(int e) { a: goto a; }/* { dg-error "new declaration" } */
class A {
  void m();
};
void A::m() { }
namespace {
  void m() { }
};
