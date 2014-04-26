/* APPLE LOCAL file radar 5277239 */
/* Test use of class method calls using property dot-syntax used for
   property 'getter' and 'setter' messaging. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#import <Foundation/Foundation.h>

/* { dg-do run } */

@interface Subclass : NSObject
+ (int)magicNumber;
+ (void)setMagicNumber:(int)value;
+ (void)setFakeSetterNumber:(int)value;
@end

@implementation Subclass
int _magicNumber = 0;
+ (int)magicNumber {
  return _magicNumber;
}

+ (void)setMagicNumber:(int)value {
  _magicNumber = value;
}

+ (void)setFakeSetterNumber:(int)value {
  _magicNumber = value;
}

+ (void) classMeth
{
	self.magicNumber = 10;
	if (self.magicNumber != 10)
	  abort ();
}
@end

int main (void) {
  
  if (Subclass.magicNumber != 0)
    abort ();
  Subclass.magicNumber = 2 /*[Subclass setMagicNumber:2]*/;
  if (Subclass.magicNumber != 2)
    abort ();

  Subclass.magicNumber += 3;
  if (Subclass.magicNumber != 5)
    abort ();
  Subclass.magicNumber -= 5;
  if (Subclass.magicNumber != 0)
    abort ();
  /* We only have a setter in the following case. */
  Subclass.fakeSetterNumber = 123;
  /* We read it using the other getter. */
  if (Subclass.magicNumber != 123)
   abort ();
  Subclass.fakeSetterNumber = Subclass.magicNumber;
  if (Subclass.magicNumber != 123)
   abort ();

  /* Test class methods using the new syntax. */
  [Subclass classMeth];
  return 0;
}
