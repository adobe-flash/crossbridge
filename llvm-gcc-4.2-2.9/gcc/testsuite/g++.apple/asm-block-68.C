/* APPLE LOCAL file inline asm labels in templates 6606502 */
/* { dg-do run } */ 
/* { dg-options "-fasm-blocks" } */
/* Radar 6606502 */

class Foo {
public:
  static void dummy(){}
};
 
class Foo1 {
public:
  static void dummy(){}
};
 
template<class T>
static void MyFunc() {
  asm {
      jmp $mylabel
    $mylabel:
  }
  T::dummy();
}
 
int main(int argc, char** argv)
{
  MyFunc<Foo>();
  MyFunc<Foo1>();
  return 0;
}
