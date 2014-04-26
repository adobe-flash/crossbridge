/* APPLE LOCAL file radar 6302949 */
/* { dg-options "-mmacosx-version-min=10.5" { target *-*-darwin* } } */
/* { dg-do compile } */

@interface BadPropClass // : NSObject
{
 int _awesome;
}

@property (readonly) int hello;
@property (nonatomic readonly  /* { dg-warning " property attributes must be separated by a comma" } */
	   getter=isAwesome) int _awesome; /* { dg-warning " property attributes must be separated by a comma" } */
@property (nonatomic, readonly, 
	   getter=isAwesome) int _anew;
@property (nonatomic, readonly, 
	getter=isAwesome) int _anew1;

@end

