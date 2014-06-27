#include <Foundation/Foundation.h>
#include <stdio.h>

#define D(msg) printf("parameter-passing.m:%d -- %s\n\n", __LINE__, msg)

typedef struct {
  int first, second;
} IntPair;

typedef struct {
  float first, second;
} FloatPair;

@interface A : NSObject {
  int _i_ivar;
  long long _ll_ivar;
  float _f_ivar;
  double _d_ivar;
  _Complex int _i_complex_ivar;
  _Complex float _f_complex_ivar;
  IntPair _i_pair_ivar;
  FloatPair _f_pair_ivar;
}

// Synthesized property access

@property (assign) int i_ivar;
@property (assign) long long ll_ivar;
@property (assign) float f_ivar;
@property (assign) double d_ivar;
@property (assign) _Complex int i_complex_ivar;
@property (assign) _Complex float f_complex_ivar;
@property (assign) IntPair i_pair_ivar;
@property (assign) FloatPair f_pair_ivar;

// Method calls & return types

-(void) nullary;
-(void) unary_i: (int) a0;
-(int) return_i;
-(void) unary_ll: (long long) a0;
-(long long) return_ll;
-(void) unary_f: (float) a0;
-(float) return_f;
-(void) unary_d: (double) a0;
-(double) return_d;
-(void) unary_i_complex: (_Complex int) a0;
-(_Complex int) return_i_complex;
-(void) unary_f_complex: (_Complex float) a0;
-(_Complex float) return_f_complex;
-(void) unary_i_pair: (IntPair) a0;
-(IntPair) return_i_pair;
-(void) unary_f_pair: (FloatPair) a0;
-(FloatPair) return_f_pair;

@end

@implementation A 
@synthesize i_ivar = _i_ivar;
@synthesize ll_ivar = _ll_ivar;
@synthesize f_ivar = _f_ivar;
@synthesize d_ivar = _d_ivar;
@synthesize i_complex_ivar = _i_complex_ivar;
@synthesize f_complex_ivar = _f_complex_ivar;
@synthesize i_pair_ivar = _i_pair_ivar;
@synthesize f_pair_ivar = _f_pair_ivar;

-(void) nullary {
  D(__FUNCTION__);
}

-(void) unary_i: (int) a0 {
  D(__FUNCTION__);
  printf("\ta0: %d\n",  a0);
  self.i_ivar = a0;
}

-(int) return_i {
  D(__FUNCTION__);
  int rv = self.i_ivar;
  printf("\t  returning: %d\n", rv);
  return rv;
}

-(void) unary_ll: (long long) a0 {
  D(__FUNCTION__);
  printf("\ta0: %lld\n",  a0);
  self.ll_ivar = a0;
}

-(long long) return_ll {
  D(__FUNCTION__);
  long long rv = self.ll_ivar;
  printf("\t  returning: %lld\n", rv);
  return rv;
}

-(void) unary_f: (float) a0 {
  D(__FUNCTION__);
  printf("\ta0: %f\n",  a0);
  self.f_ivar = a0;
}

-(float) return_f {
  D(__FUNCTION__);
  float rv = self.f_ivar;
  printf("\t  returning: %f\n", rv);
  return rv;
}

-(void) unary_d: (double) a0 {
  D(__FUNCTION__);
  printf("\ta0: %f\n",  a0);
  self.d_ivar = a0;
}

-(double) return_d {
  D(__FUNCTION__);
  double rv = self.d_ivar;
  printf("\t  returning: %f\n", rv);
  return rv;
}

-(void) unary_i_complex: (_Complex int) a0 {
  D(__FUNCTION__);
  printf("\ta0: %d + %dj\n",  __real a0, __imag a0);
  self.i_complex_ivar = a0;
}

-(_Complex int) return_i_complex {
  D(__FUNCTION__);
  _Complex int rv = self.i_complex_ivar;
  printf("\t  returning: (%d, %d)\n", __real rv, __imag rv);
  return rv;
}

-(void) unary_f_complex: (_Complex float) a0 {
  D(__FUNCTION__);
  printf("\ta0: %f + %fj\n",  __real a0, __imag a0);
  self.f_complex_ivar = a0;
}

-(_Complex float) return_f_complex {
  D(__FUNCTION__);
  _Complex float rv = self.f_complex_ivar;
  printf("\t  returning: (%f, %f)\n", __real rv, __imag rv);
  return rv;
}

-(void) unary_i_pair: (IntPair) a0 {
  D(__FUNCTION__);
  printf("\ta0: (%d, %d)\n",  a0.first, a0.second);
  self.i_pair_ivar = a0;
}

-(IntPair) return_i_pair {
  D(__FUNCTION__);
  IntPair rv = self.i_pair_ivar;
  printf("\t  returning: (%d, %d)\n", rv.first, rv.second);
  return rv;
}

-(void) unary_f_pair: (FloatPair) a0 {
  D(__FUNCTION__);
  printf("\ta0: (%f, %f)\n",  a0.first, a0.second);
  self.f_pair_ivar = a0;
}

-(FloatPair) return_f_pair {
  D(__FUNCTION__);
  FloatPair rv = self.f_pair_ivar;
  printf("\t  returning: (%f, %f)\n", rv.first, rv.second);
  return rv;
}

@end

int main() {
  NSAutoreleasePool *AP = [[NSAutoreleasePool alloc] init];
  
  A *a = [[A alloc] init];

  int i_test_var = 0xABCDABCD;
  long long ll_test_var = 0xABCDABCDABCDABCDLL;
  float f_test_var = 52.5;
  double d_test_var = 25.2;
  _Complex int i_complex_test_var = 123 + 456j;
  _Complex float f_complex_test_var = 123.4 + 678.5j;
  IntPair i_pair_test_var = { 0xABCDABCD, 0xDBCADBCA };
  FloatPair f_pair_test_var = { 25.2, 52.5 };

  [a nullary];
  [a unary_i: i_test_var];
  [a unary_ll: ll_test_var];
  [a unary_f: f_test_var];
  [a unary_d: d_test_var];
  [a unary_i_complex: i_complex_test_var];
  [a unary_f_complex: f_complex_test_var];
  [a unary_i_pair: i_pair_test_var];
  [a unary_f_pair: f_pair_test_var];

  {
    int rv = [a return_i];
    printf("\tresult: %d\n\n", rv);
    assert(rv == i_test_var);
  }
  {
    long long rv = [a return_ll];
    printf("\tresult: %lld\n\n", rv);
    assert(rv == ll_test_var);
  }
  {
    float rv = [a return_f];
    printf("\tresult: %f\n\n", rv);
    assert(rv == f_test_var);
  }
  {
    double rv = [a return_d];
    printf("\tresult: %f\n\n", rv);
    assert(rv == d_test_var);
  }
  {
    _Complex int rv = [a return_i_complex];
    printf("\tresult: (%d, %d)\n\n", __real rv, __imag rv);
    assert(rv == i_complex_test_var);
  }
  {
    _Complex float rv = [a return_f_complex];
    printf("\tresult: (%f, %f)\n\n", __real rv, __imag rv);
    assert(rv == f_complex_test_var);
  }
  {
    IntPair rv = [a return_i_pair];
    printf("\tresult: (%d, %d)\n\n", rv.first, rv.second);
    assert(memcmp(&rv, &i_pair_test_var, sizeof(rv)) == 0);
  }
  {
    FloatPair rv = [a return_f_pair];
    printf("\tresult: (%f, %f)\n\n", rv.first, rv.second);
    assert(memcmp(&rv, &f_pair_test_var, sizeof(rv)) == 0);
  }

  [a release];

  [AP release];
  return 0;
}
