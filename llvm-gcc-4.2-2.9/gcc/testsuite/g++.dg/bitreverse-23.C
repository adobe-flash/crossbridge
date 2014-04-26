/* APPLE LOCAL file 4430139 */
extern "C" void abort();
/* { dg-do run { target powerpc*-*-darwin* } } */
/* Check that layout works in presence of things other than
   data members. */
#pragma reverse_bitfields on
#pragma ms_struct on
class X {
  public:
    int x : 8;
    static int y;
    int z : 8;
    enum a { b, c, d};
    int q : 8;
    enum aa { bb, cc, dd};
};
union U { X x; int z[5]; };
int main() {
    U xx;
    xx.x.x = 5;
    xx.x.z = 7;
    xx.x.q = 9;
    if ((xx.z[0] & 0xFFFFFF) != 0x090705)
      abort();
    return 0;
}
