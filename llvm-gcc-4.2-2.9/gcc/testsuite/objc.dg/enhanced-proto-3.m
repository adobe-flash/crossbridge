/* APPLE LOCAL file radar 4550413 */
/* Use of @optional protocol in messaing must be allowed. */
/* { dg-do compile } */

@protocol MyProto1 
@optional
- (void) OPT;
@required 
- (void) REQ;
@optional
+ (void) CLS_OPT;
@end

@interface MyClass <MyProto1>
@end

@implementation MyClass
- (void) REQ {}
@end

int main()
{
	id <MyProto1> p;
	[p REQ];
	[p OPT];
	[MyClass  CLS_OPT];
}
