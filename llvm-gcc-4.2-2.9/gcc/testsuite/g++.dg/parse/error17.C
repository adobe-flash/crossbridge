// PR c++/16965

// LLVM LOCAL
template <typename T> struct B { // { dg-error "" }
  static int Bar(T); // { dg-error "" }
}; 
struct D : B<int>, B<char> {}; 
 
int i2 = D::Bar(2); // { dg-error "" }
