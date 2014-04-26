/* APPLE LOCAL file 4781080 */
/* 
  Test that for x86_64:
  float return: use objc_msgSend()    (this is new)
  double return: use objc_msgSend()    (this is new)
  long-double return: use objc_msgSend_fpret()    (this doesn't change)
*/
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-options "-fnext-runtime -m64 -fobjc-abi-version=1" } */

@interface Test { id isa; } @end
@implementation Test
+(void)initialize { }
+(float) float_return { return 0; }
+(double) double_return { return 0; }
+(long double) longdouble_return { return 0; }
@end

int main() {
    float f = [Test float_return];  // should use objc_msgSend on x86_64
    double d = [Test double_return];  // should use objc_msgSend on x86_64
    long double ld = [Test longdouble_return];  // should use objc_msgSend_fpret on x86_64
    return f+d+ld;
}

/* { dg-final { scan-assembler "call\t_objc_msgSend" } } */
/* { dg-final { scan-assembler "call\t_objc_msgSend" } } */
/* { dg-final { scan-assembler "call\t_objc_msgSend_fpret" } } */
