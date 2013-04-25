/* { dg-options "-fpreprocessed -g" } */
/* { dg-compile } */

# 1 "foo.cpp"
# 1 "one.cpp.incl"
# 56 "one.cpp.incl" 1
# 1 "two.hpp" 1
# 12 "two.hpp" 
class A {
 public:
  static void* fn();
# 1 "four.incl" 1
# 1 "five.hpp" 1
public:
  static int fn2() {
    return 0;
  }
# 1 "four.incl" 2
# 25 "two.hpp" 2
  static inline int fn3() { return d1; }
 private:
  static int d1;
};
# 57 "one.cpp.incl" 2
