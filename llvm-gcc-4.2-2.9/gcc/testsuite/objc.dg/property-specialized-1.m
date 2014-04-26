/* APPLE LOCAL file radar 5218071 */
/* Test that property type in the derived class can be more specialized than
   that of its base class.
*/
/* { dg-options "-mmacosx-version-min=10.5" } */
/* { dg-do compile } */

@protocol P1 @end
@protocol P2 @end
@protocol P3 @end

@interface NSData @end

@interface MutableNSData : NSData @end

@interface Base 
@property(readonly) id ref;
@property(readonly) id another_ref;
@property (readonly) NSData *nsdata;
@end

@interface Derived : Base
@property (readonly, assign) NSData *ref;
@property(readonly) NSData * another_ref;
@property (readonly) NSData * nsdata;
@end
